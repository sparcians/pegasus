#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"
#include "system/AtlasSystem.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "arch/register_macros.hpp"
#include "include/AtlasTypes.hpp"

//Core database headers
#include "simdb/ObjectManager.hpp"
#include "simdb/ObjectRef.hpp"
#include "simdb/TableRef.hpp"
#include "simdb/utils/ObjectQuery.hpp"
#include "simdb/utils/BlobHelpers.hpp"
#include "simdb/async/AsyncTaskEval.hpp"

//SQLite-specific headers
#include "simdb/impl/sqlite/SQLiteConnProxy.hpp"
#include "simdb/impl/sqlite/TransactionUtils.hpp"
#include "simdb/impl/sqlite/Errors.hpp"
#include <sqlite3.h>

namespace atlas
{
    AtlasState::AtlasState(sparta::TreeNode* core_node, const AtlasStateParameters* p) :
        sparta::Unit(core_node),
        hart_id_(p->hart_id),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        inst_logger_(core_node),
        stop_sim_action_group_("stop_sim")
    {
        auto json_dir = (xlen_ == 32) ? REG32_JSON_DIR : REG64_JSON_DIR;
        int_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_int.json"), "int_regs");
        fp_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_fp.json"), "fp_regs");
        vec_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_vec.json"), "vec_regs");
        csr_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_csr.json"), "csr_regs");

        // Increment PC Action
        increment_pc_action_ =
            atlas::Action::createAction<&AtlasState::incrementPc_>(this, "increment pc");

        // Create Action to stop simulation
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);
    }

    // Not default -- defined in source file to reduce massive inlining
    AtlasState::~AtlasState() {}

    void AtlasState::onBindTreeEarly_()
    {
        fetch_unit_ = getContainer()->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = getContainer()->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = getContainer()->getChild("translate")->getResourceAs<Translate*>();
        if (auto exc = getContainer()->getChild("exception", false)) {
            exception_unit_ = exc->getResourceAs<Exception*>();
        } else {
            exception_unit_ = nullptr;
        }
    }

    template <typename MemoryType> MemoryType AtlasState::readMemory(const Addr paddr)
    {
        auto* memory = atlas_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        std::vector<uint8_t> buffer(sizeof(MemoryType) / sizeof(uint8_t), 0);
        const bool success = memory->tryRead(paddr, size, buffer.data());
        sparta_assert(success, "Failed to read from memory at address 0x" << std::hex << paddr);

        const MemoryType value = convertFromByteVector<MemoryType>(buffer);
        ILOG("Memory read to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
        return value;
    }

    template <typename MemoryType>
    void AtlasState::writeMemory(const Addr paddr, const MemoryType value)
    {
        auto* memory = atlas_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const bool success = memory->tryWrite(paddr, size, buffer.data());
        sparta_assert(success, "Failed to write to memory at address 0x" << std::hex << paddr);

        ILOG("Memory write to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
    }

    template int8_t AtlasState::readMemory<int8_t>(const Addr);
    template uint8_t AtlasState::readMemory<uint8_t>(const Addr);
    template int16_t AtlasState::readMemory<int16_t>(const Addr);
    template uint16_t AtlasState::readMemory<uint16_t>(const Addr);
    template int32_t AtlasState::readMemory<int32_t>(const Addr);
    template uint32_t AtlasState::readMemory<uint32_t>(const Addr);
    template int64_t AtlasState::readMemory<int64_t>(const Addr);
    template uint64_t AtlasState::readMemory<uint64_t>(const Addr);
    template void AtlasState::writeMemory<uint8_t>(const Addr, const uint8_t);
    template void AtlasState::writeMemory<uint16_t>(const Addr, const uint16_t);
    template void AtlasState::writeMemory<uint32_t>(const Addr, const uint32_t);
    template void AtlasState::writeMemory<uint64_t>(const Addr, const uint64_t);

    void AtlasState::insertExecuteActions(ActionGroup* action_group)
    {
        // FIXME: InstructionLogger's enable method doesn't work when using a BaseObserver
        // pointer...
        if (inst_logger_.enabled())
        {
            inst_logger_.insertPreExecuteAction(action_group);
            inst_logger_.insertPostExecuteAction(action_group);
        }

        for (auto observer : observers_)
        {
            if (observer->enabled())
            {
                observer->insertPreExecuteAction(action_group);
                observer->insertPostExecuteAction(action_group);
            }
        }

        // Increment PC and upate simulation state
        action_group->insertActionAfter(increment_pc_action_, ActionTags::EXECUTE_TAG);

        // Allow the Exception unit to deal with any unhandled exceptions
        exception_unit_->insertExecuteActions(action_group);
    }

    ActionGroup* AtlasState::incrementPc_(AtlasState*)
    {
        // Set PC
        pc_ = next_pc_;
        next_pc_ = 0;

        // Increment instruction count
        ++sim_state_.inst_count;

        return nullptr;
    }

    void AtlasState::enableCoSimDebugger(std::shared_ptr<simdb::ObjectManager> db,
                                         std::shared_ptr<CoSimQuery> query)
    {
        bool valid = inst_logger_.enabled();
        if (!valid) {
            for (auto obs : observers_) {
                if (obs->enabled()) {
                    valid = true;
                    break;
                }
            }
        }

        sparta_assert(valid, "No observers enabled, nothing to debug!");
        cosim_db_ = db;
        cosim_query_ = query;

        auto tbl = cosim_db_->getTable("Registers");

        auto int_rset = getIntRegisterSet();
        for (int reg_idx = 0; reg_idx < (int)int_rset->getNumRegisters(); ++reg_idx) {
            auto reg = int_rset->getRegister(reg_idx);
            auto record = tbl->createObjectWithArgs("HartId", (int)getHartId(), "RegType", 0, "RegIdx", reg_idx, "InitVal", reg->dmiRead<uint64_t>());
            auto reg_name = "x" + std::to_string(reg_idx);
            reg_ids_by_name_[reg_name] = record->getId();
        }

        auto fp_rset = getFpRegisterSet();
        for (int reg_idx = 0; reg_idx < (int)fp_rset->getNumRegisters(); ++reg_idx) {
            auto reg = fp_rset->getRegister(reg_idx);
            auto record = tbl->createObjectWithArgs("HartId", (int)getHartId(), "RegType", 1, "RegIdx", reg_idx, "InitVal", reg->dmiRead<uint64_t>());
            auto reg_name = "f" + std::to_string(reg_idx);
            reg_ids_by_name_[reg_name] = record->getId();
        }

        auto vec_rset = getVecRegisterSet();
        for (int reg_idx = 0; reg_idx < (int)vec_rset->getNumRegisters(); ++reg_idx) {
            auto reg = vec_rset->getRegister(reg_idx);
            auto record = tbl->createObjectWithArgs("HartId", (int)getHartId(), "RegType", 2, "RegIdx", reg_idx, "InitVal", reg->dmiRead<uint64_t>());
            auto reg_name = "v" + std::to_string(reg_idx);
            reg_ids_by_name_[reg_name] = record->getId();
        }

        auto csr_rset = getCsrRegisterSet();
        for (int reg_idx = 0; reg_idx < (int)csr_rset->getNumRegisters(); ++reg_idx) {
            if (auto reg = csr_rset->getRegister(reg_idx)) {
                auto record = tbl->createObjectWithArgs("HartId", (int)getHartId(), "RegType", 3, "RegIdx", reg_idx, "InitVal", reg->dmiRead<uint64_t>());
                auto reg_name = "csr" + std::to_string(reg_idx);
                reg_ids_by_name_[reg_name] = record->getId();
            }
        }
    }

    class InstSnapshotter : public simdb::WorkerTask
    {
    public:
        InstSnapshotter(simdb::ObjectManager* obj_mgr, int hart,
                        const std::string& rs1_name, uint64_t rs1_val,
                        const std::string& rs2_name, uint64_t rs2_val,
                        const std::string& rd_name, uint64_t rd_val_before, uint64_t rd_val_after,
                        int has_imm, uint64_t imm,
                        const std::string& disasm,
                        const std::string& mnemonic,
                        uint64_t opcode, uint64_t pc, int priv, int result_code)
            : obj_mgr_(obj_mgr)
            , hart_(hart)
            , rs1_name_(rs1_name)
            , rs1_val_(rs1_val)
            , rs2_name_(rs2_name)
            , rs2_val_(rs2_val)
            , rd_name_(rd_name)
            , rd_val_before_(rd_val_before)
            , rd_val_after_(rd_val_after)
            , has_imm_(has_imm)
            , imm_(imm)
            , disasm_(disasm)
            , mnemonic_(mnemonic)
            , opcode_(opcode)
            , pc_(pc)
            , priv_(priv)
            , result_code_(result_code)
        {}

        void completeTask() override
        {
            auto tbl = obj_mgr_->getTable("Instructions");

            tbl->createObjectWithArgs("HartId", hart_,
                                      "Rs1", rs1_name_,
                                      "Rs1Val", rs1_val_,
                                      "Rs2", rs2_name_,
                                      "Rs2Val", rs2_val_,
                                      "Rd", rd_name_,
                                      "RdValBefore", rd_val_before_,
                                      "RdValAfter", rd_val_after_,
                                      "HasImm", has_imm_,
                                      "Imm", imm_,
                                      "Disasm", disasm_,
                                      "Mnemonic", mnemonic_,
                                      "Opcode", opcode_,
                                      "PC", pc_,
                                      "Priv", priv_,
                                      "ResultCode", result_code_);
        }

    private:
        simdb::ObjectManager* obj_mgr_;
        int hart_;
        std::string rs1_name_;
        uint64_t rs1_val_;
        std::string rs2_name_;
        uint64_t rs2_val_;
        std::string rd_name_;
        uint64_t rd_val_before_;
        uint64_t rd_val_after_;
        int has_imm_;
        uint64_t imm_;
        std::string disasm_;
        std::string mnemonic_;
        uint64_t opcode_;
        uint64_t pc_;
        int priv_;
        int result_code_;
    };

    void AtlasState::snapshotAndSyncWithCoSim()
    {
        if (SPARTA_EXPECT_TRUE(!cosim_db_)) {
            return;
        }

        const AtlasInstPtr & insn = getCurrentInst();

        int hart = getHartId();
        std::string rs1_name = insn->hasRs1() ? insn->getRs1()->getName() : "";
        uint64_t rs1_val = insn->hasRs1() ? insn->getRs1()->dmiRead<uint64_t>() : 0;
        std::string rs2_name = insn->hasRs2() ? insn->getRs2()->getName() : "";
        uint64_t rs2_val = insn->hasRs2() ? insn->getRs2()->dmiRead<uint64_t>() : 0;
        std::string rd_name = insn->hasRd() ? insn->getRd()->getName() : "";

        uint64_t rd_val_before = 0;
        uint64_t rd_val_after = 0;
        if (insn->hasRd()) {
            Observer* obs = nullptr;
            if (inst_logger_.enabled()) {
                obs = &inst_logger_;
            } else {
                for (auto observer : observers_) {
                    if (observer->enabled()) {
                        obs = observer;
                        break;
                    }
                }
            }

            sparta_assert(obs, "No observers enabled, nothing to debug!");
            rd_val_before = obs->getPrevRdValue();
            rd_val_after = insn->getRd()->dmiRead<uint64_t>();

            // Force overwrite any differences to keep the cosim going.
            // This builds a complete IDE backend despite there being
            // problems in the code such as unimplemented instruction
            // handlers.
            if (rd_val_before != rd_val_after) {
                // yyy
            }
        }

        int has_imm = insn->hasImmediate() ? 1 : 0;
        uint64_t imm = insn->hasImmediate() ? insn->getImmediate() : 0;
        std::string disasm = insn->dasmString();
        std::string mnemonic = insn->getMnemonic();
        uint64_t opcode = insn->getOpcode();
        uint64_t pc = getPc();
        int priv = (int)priv_mode_;
        int result_code = 0; // yyy

        std::unique_ptr<simdb::WorkerTask> task(new InstSnapshotter(cosim_db_.get(), hart,
                                                                    rs1_name, rs1_val,
                                                                    rs2_name, rs2_val,
                                                                    rd_name, rd_val_before, rd_val_after,
                                                                    has_imm, imm,
                                                                    disasm, mnemonic,
                                                                    opcode, pc, priv, result_code));

        cosim_db_->getTaskQueue()->addWorkerTask(std::move(task));
    }

    uint64_t AtlasState::getMStatusInitialValue(const AtlasState* state, const uint64_t xlen_val)
    {
        // yyy cnyce
        (void)state; (void)xlen;
        return 42949672960;
    }
} // namespace atlas
