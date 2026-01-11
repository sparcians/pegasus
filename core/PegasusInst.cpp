#include "core/PegasusInst.hpp"
#include "core/PegasusCore.hpp"
#include "sparta/utils/SpartaTester.hpp"

namespace pegasus
{
    const PegasusExtractorPtr testExtractorPointer(const PegasusExtractorPtr & extractor_info,
                                                   const std::string & mnemonic)
    {
        sparta_assert(extractor_info, "Invalid PegasusExtractor pointer! " << mnemonic);
        return extractor_info;
    }

    translate_types::AccessType
    determineMemoryAccessType(const PegasusExtractorPtr & extractor_info, const bool is_store,
                              const mavis::InstructionUniqueID mavis_uid)
    {
        if (is_store)
        {
            return translate_types::AccessType::STORE;
        }
        else if (extractor_info->isMemoryInst())
        {
            if (SPARTA_EXPECT_FALSE(extractor_info->isHypervisorInst()))
            {
                if ((mavis_uid == PegasusCore::MavisUIDs::MAVIS_UID_HLVX_HU)
                    || (mavis_uid == PegasusCore::MavisUIDs::MAVIS_UID_HLVX_WU))
                {
                    return translate_types::AccessType::EXECUTE;
                }
            }
            return translate_types::AccessType::LOAD;
        }

        // Not a load or store inst
        return translate_types::AccessType::INVALID;
    }

    uint64_t getImmediateValue(const mavis::OpcodeInfo::PtrType & opcode_info)
    {
        return opcode_info->getImmediateType() == mavis::ImmediateType::SIGNED
                   ? opcode_info->getSignedOffset()
                   : opcode_info->getImmediate();
    }

    template <mavis::InstMetaData::OperandFieldID OperandFieldId>
    const mavis::OperandInfo::Element*
    getOperand(const mavis::OperandInfo::ElementList & operand_list)
    {
        const auto operand_it = std::find_if(operand_list.begin(), operand_list.end(),
                                             [](const mavis::OperandInfo::Element & operand)
                                             { return operand.field_id == OperandFieldId; });

        // Instruction does not have this operand type
        if (operand_it == operand_list.end())
        {
            return nullptr;
        }
        else
        {
            return &(*operand_it);
        }
    }

    PegasusInst::PegasusInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                             const PegasusExtractorPtr & extractor_info, PegasusState* state) :
        opcode_info_(opcode_info),
        extractor_info_(testExtractorPointer(extractor_info, getMnemonic())),
        opcode_size_(((getOpcode() & 0x3) != 0x3) ? 2 : 4),
        is_store_type_(opcode_info->isInstType(mavis::OpcodeInfo::InstructionTypes::STORE)),
        memory_access_type_(
            determineMemoryAccessType(extractor_info_, is_store_type_, getMavisUid())),
        immediate_value_(getImmediateValue(opcode_info)),
        veccfg_overrides_(extractor_info->veccfg_),
        rs1_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS1>(
            opcode_info->getSourceOpInfoList())),
        rs2_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS2>(
            opcode_info->getSourceOpInfoList())),
        rs3_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS3>(
            opcode_info->getSourceOpInfoList())),
        rd_info_(
            getOperand<mavis::InstMetaData::OperandFieldID::RD>(opcode_info->getDestOpInfoList())),
        rd2_info_(
            getOperand<mavis::InstMetaData::OperandFieldID::RD2>(opcode_info->getDestOpInfoList())),
        rs1_reg_(state->getSpartaRegister(rs1_info_)),
        rs2_reg_(state->getSpartaRegister(rs2_info_)),
        rs3_reg_(state->getSpartaRegister(rs3_info_)),
        rd_reg_(state->getSpartaRegister(rd_info_)),
        rd2_reg_(state->getSpartaRegister(rd2_info_)),
        inst_action_group_(extractor_info_->inst_action_group_)
    {
    }

    PegasusInst::~PegasusInst() = default;

    bool PegasusInst::writesCsr() const
    {
        switch (getMavisUid())
        {
            // csrrw/csrrwi always writes
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRW:
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRWI:
                return true;
            // csrrs/csrrc write if rs1 != 0
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRS:
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRC:
                return rs1_info_ && (rs1_info_->field_value != 0);
            // csrrsi/csrrci writes if imm != 0
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRSI:
            case PegasusCore::MavisUIDs::MAVIS_UID_CSRRCI:
                return getImmediate() != 0;
            default:
                sparta_assert(hasCsr() == false, "Unknown instruction with CSR: " << *this);
                return false;
        }
    }

    void PegasusInst::updateVecConfig(const PegasusState* state)
    {
        vec_config_ = makeVecCfg(*state->getVectorConfig(), veccfg_overrides_);
    }

    template <bool IS_UNIT_TEST> bool PegasusInst::compare(const PegasusInst* inst) const
    {
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(uid_, inst->uid_);
            EXPECT_EQUAL(getOpcode(), inst->getOpcode());
            EXPECT_EQUAL(getOpcodeSize(), inst->getOpcodeSize());
            EXPECT_EQUAL(isMemoryInst(), inst->isMemoryInst());
            EXPECT_EQUAL(isChangeOfFlowInst(), inst->isChangeOfFlowInst());
            EXPECT_EQUAL(hasCsr(), inst->hasCsr());
            EXPECT_EQUAL(writesCsr(), inst->writesCsr());
            EXPECT_EQUAL(isStoreType(), inst->isStoreType());
            EXPECT_EQUAL(getMnemonic(), inst->getMnemonic());
            EXPECT_EQUAL(dasmString(), inst->dasmString());
        }
        else
        {
            if (uid_ != inst->uid_)
            {
                return false;
            }
            if (getOpcode() != inst->getOpcode())
            {
                return false;
            }
            if (getOpcodeSize() != inst->getOpcodeSize())
            {
                return false;
            }
            if (isMemoryInst() != inst->isMemoryInst())
            {
                return false;
            }
            if (isChangeOfFlowInst() != inst->isChangeOfFlowInst())
            {
                return false;
            }
            if (hasCsr() != inst->hasCsr())
            {
                return false;
            }
            if (writesCsr() != inst->writesCsr())
            {
                return false;
            }
            if (isStoreType() != inst->isStoreType())
            {
                return false;
            }
            if (getMnemonic() != inst->getMnemonic())
            {
                return false;
            }
            if (dasmString() != inst->dasmString())
            {
                return false;
            }
        }

        return true;
    }

    template bool PegasusInst::compare<false>(const PegasusInst* inst) const;
    template bool PegasusInst::compare<true>(const PegasusInst* inst) const;

    std::ostream & operator<<(std::ostream & os, const PegasusInst & inst)
    {
        os << "uid: " << std::dec << inst.uid_ << " " << inst.dasmString() << " "
           << inst.inst_action_group_;
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const PegasusInstPtr & inst)
    {
        os << *inst;
        return os;
    }
} // namespace pegasus
