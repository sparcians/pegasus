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
        stop_action_ = atlas::Action::createAction<&AtlasState::stopSim_>(this, "stop sim");
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);

        // Note that just because we are adding the InstructionLogger right here
        // doesn't mean that it will be enabled. The user must enable it at the
        // command prompt with the "-l" option.
        observers_.emplace_back(std::make_unique<InstructionLogger>(core_node));
    }

    // Not default -- defined in source file to reduce massive inlining
    AtlasState::~AtlasState() {}

    void AtlasState::onBindTreeEarly_()
    {
        fetch_unit_ = getContainer()->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = getContainer()->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = getContainer()->getChild("translate")->getResourceAs<Translate*>();

        // FIXME: Add the Exception unit to the various unit tests like Fetch/Execute/Translate
        if (auto exc = getContainer()->getChild("exception", false)) {
            exception_unit_ = exc->getResourceAs<Exception*>();
        } else {
            exception_unit_ = nullptr;
        }
    }

    // Check all PC/reg/csr values against our cosim comparator,
    // and return the result code as follows:
    //
    //   success            0x00
    //   exception          0x1x (x encodes the exception cause)
    //   pc mismatch        0x2- (- means ignored)
    //   reg val mismatch   0x3-
    //   unimplemented inst 0x4-
    //
    // At the end of this method, all PC/reg/csr values will be
    // synced with the other simulation ("truth").
    int AtlasState::compareWithCoSimAndSync_()
    {
        sparta::utils::ValidValue<int> rc;
        const auto& insn = getCurrentInst();
        if (insn->unimplemented()) {
            // status 0x4 means "unimplemented but passing reg val checks"
            rc = 0x4 << 16;
        }

        const auto& exc = exception_unit_->getUnhandledException();
        if (!rc.isValid() && exc.isValid()) {
            rc = (1 << 16) | static_cast<int>(exc.getValue());
        }

        auto atlas_pc = getPc();
        auto cosim_pc = cosim_query_->getExpectedPC(getHartId());
        if (atlas_pc != cosim_pc) {
            if (!rc.isValid()) {
                rc = 0x2 << 16;
            } else if (insn->unimplemented()) {
                // status 0x5 means "unimplemented and fails reg val checks"
                rc = 0x5 << 16;
            }
            // Force overwrite to keep the cosim going.
            pc_ = cosim_pc;
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumIntRegisters(); ++reg_idx) {
            auto atlas_reg = int_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getIntRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val) {
                if (!rc.isValid()) {
                    rc = 0x3 << 16;
                } else if (insn->unimplemented()) {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumFpRegisters(); ++reg_idx) {
            auto atlas_reg = fp_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getFpRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val) {
                if (!rc.isValid()) {
                    rc = 0x3 << 16;
                } else if (insn->unimplemented()) {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumVecRegisters(); ++reg_idx) {
            auto atlas_reg = vec_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getVecRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val) {
                if (!rc.isValid()) {
                    rc =0x3 << 16;
                } else if (insn->unimplemented()) {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < csr_rset_->getNumRegisters(); ++reg_idx) {
            if (auto atlas_reg = csr_rset_->getRegister(reg_idx)) {
                if (cosim_query_->isCsrImplemented(atlas_reg->getName())) {
                    auto atlas_val = atlas_reg->dmiRead<uint64_t>();
                    auto cosim_val = cosim_query_->getCsrRegValue(getHartId(), atlas_reg->getName());

                    if (atlas_val != cosim_val) {
                        // We should only count the CSR mismatches during exceptions
                        if (!rc.isValid() && exc.isValid()) {
                            rc = 0x3 << 16;
                        } else if (insn->unimplemented()) {
                            // status 0x5 means "unimplemented and fails reg val checks"
                            rc = 0x5 << 16;
                        }
                        // Force overwrite to keep the cosim going.
                        atlas_reg->dmiWrite(cosim_val);
                    }
                }
            }
        }

        if (rc.isValid()) {
            return rc;
        }
        return 0;
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
        for (auto& observer : observers_)
        {
            if (observer->enabled())
            {
                observer->insertExecuteActions(action_group);
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
                                         std::shared_ptr<CoSimQuery> query,
                                         const std::vector<RegisterInfo> &reg_info)
    {
        bool valid = false;
        for (auto& obs : observers_) {
            if (obs->enabled()) {
                valid = true;
                break;
            }
        }

        sparta_assert(valid, "No observers enabled, nothing to debug!");
        cosim_db_ = db;
        cosim_query_ = query;

        auto tbl = cosim_db_->getTable("Registers");

        for (const auto& tup : reg_info) {
            const auto& reg_name = std::get<0>(tup);
            const auto group_num = std::get<1>(tup);
            const auto reg_id = std::get<2>(tup);
            const auto expected_val = std::get<3>(tup);
            const auto actual_val = std::get<4>(tup);

            tbl->createObjectWithArgs("HartId", (int)getHartId(),
                                      "RegName", reg_name,
                                      "RegType", (int)group_num,
                                      "RegIdx", (int)reg_id,
                                      "ExpectedInitVal", expected_val,
                                      "ActualInitVal", actual_val);

            switch (group_num) {
                case 0:
                    getIntRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 1:
                    getFpRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 2:
                    getVecRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 3:
                    getCsrRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                default:
                    throw sparta::SpartaException("Invalid group num!");
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
                        uint64_t cosim_rd_val_after,
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
            , cosim_rd_val_after_(cosim_rd_val_after)
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
                                      "TruthRdValAfter", cosim_rd_val_after_,
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
        uint64_t cosim_rd_val_after_;
        int has_imm_;
        uint64_t imm_;
        std::string disasm_;
        std::string mnemonic_;
        uint64_t opcode_;
        uint64_t pc_;
        int priv_;
        int result_code_;
    };

    class CsrValuesSnapshotter : public simdb::WorkerTask
    {
    public:
        CsrValuesSnapshotter(simdb::ObjectManager* obj_mgr, int hart, uint64_t pc, const std::vector<std::tuple<std::string, uint64_t, uint64_t>> &all_csr_vals)
            : obj_mgr_(obj_mgr)
            , hart_(hart)
            , pc_(pc)
            , all_csr_vals_(all_csr_vals)
        {}

        void completeTask() override
        {
            auto tbl = obj_mgr_->getTable("PostInstCsrVals");
            for (const auto& tup : all_csr_vals_) {
                const auto& csr_name = std::get<0>(tup);
                const auto atlas_csr_val = std::get<1>(tup);
                const auto cosim_csr_val = std::get<2>(tup);

                tbl->createObjectWithArgs("HartId", hart_,
                                          "PC", pc_,
                                          "CsrName", csr_name,
                                          "AtlasCsrVal", atlas_csr_val,
                                          "CosimCsrVal", cosim_csr_val);
            }
        }

    private:
        simdb::ObjectManager* obj_mgr_;
        int hart_;
        uint64_t pc_;
        std::vector<std::tuple<std::string, uint64_t, uint64_t>> all_csr_vals_;
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
        uint64_t cosim_rd_val_after = 0;

        if (insn->hasRd()) {
            Observer* obs = nullptr;
            for (auto& observer : observers_) {
                if (observer->enabled()) {
                    obs = observer.get();
                    break;
                }
            }

            sparta_assert(obs, "No observers enabled, nothing to debug!");

            auto rd = insn->getRd();
            rd_val_before = obs->getPrevRdValue();
            rd_val_after = rd->dmiRead<uint64_t>();

            switch (rd->getGroupNum()) {
            case 0:
                // INT
                cosim_rd_val_after = cosim_query_->getIntRegValue(getHartId(), rd->getID());
                break;
            case 1:
                // FP
                cosim_rd_val_after = cosim_query_->getFpRegValue(getHartId(), rd->getID());
                break;
            case 2:
                // VEC
                cosim_rd_val_after = cosim_query_->getVecRegValue(getHartId(), rd->getID());
                break;
            case 3:
                // Let this go to the default case assert. CSRs should not be written to in RD.
            default:
                sparta_assert(false, "Invalid register group num!");
                break;
            }
        }

        int has_imm = insn->hasImmediate() ? 1 : 0;
        uint64_t imm = insn->hasImmediate() ? insn->getImmediate() : 0;
        std::string disasm = insn->dasmString();
        std::string mnemonic = insn->getMnemonic();
        uint64_t opcode = insn->getOpcode();
        uint64_t pc = getPc();
        int priv = (int)priv_mode_;

        // Capture the CSR values from both simulators after processing an exception.
        std::vector<std::tuple<std::string, uint64_t, uint64_t>> all_csr_vals;
        if (exception_unit_->getUnhandledException().isValid()) {
            for (uint32_t reg_idx = 0; reg_idx < csr_rset_->getNumRegisters(); ++reg_idx) {
                if (auto atlas_reg = csr_rset_->getRegister(reg_idx)) {
                    if (cosim_query_->isCsrImplemented(atlas_reg->getName())) {
                        auto atlas_val = atlas_reg->dmiRead<uint64_t>();
                        auto cosim_val = cosim_query_->getCsrRegValue(getHartId(), atlas_reg->getName());
                        all_csr_vals.push_back(std::make_tuple(atlas_reg->getName(), atlas_val, cosim_val));
                    }
                }
            }
        }

        int result_code = compareWithCoSimAndSync_();

        std::unique_ptr<simdb::WorkerTask> task(new InstSnapshotter(cosim_db_.get(), hart,
                                                                    rs1_name, rs1_val,
                                                                    rs2_name, rs2_val,
                                                                    rd_name, rd_val_before, rd_val_after,
                                                                    cosim_rd_val_after,
                                                                    has_imm, imm,
                                                                    disasm, mnemonic,
                                                                    opcode, pc, priv, result_code));

        cosim_db_->getTaskQueue()->addWorkerTask(std::move(task));

        if (!all_csr_vals.empty()) {
            task.reset(new CsrValuesSnapshotter(cosim_db_.get(), hart, pc, all_csr_vals));
            cosim_db_->getTaskQueue()->addWorkerTask(std::move(task));
        }
    }

    uint64_t AtlasState::getMStatusInitialValue(const AtlasState* state, const uint64_t xlen_val)
    {
        // TODO cnyce
        (void)state; (void)xlen;
        return 42949672960;
    }
} // namespace atlas
