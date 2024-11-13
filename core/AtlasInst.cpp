#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    template <mavis::InstMetaData::OperandFieldID OperandFieldId>
    AtlasRegisterPtr getAtlasReg(const AtlasState* state,
                                 const mavis::OperandInfo::ElementList & operand_list)
    {
        const auto operand = std::find_if(operand_list.begin(), operand_list.end(),
                                          [](const mavis::OperandInfo::Element & operand)
                                          { return operand.field_id == OperandFieldId; });

        // Instruction does not have this operand type
        if (operand == operand_list.end())
        {
            return nullptr;
        }

        // Determine Atlas register type
        RegType reg_type = RegType::INVALID;
        switch (operand->operand_type)
        {
            case mavis::InstMetaData::OperandTypes::WORD:
            case mavis::InstMetaData::OperandTypes::LONG:
                reg_type = RegType::INTEGER;
                break;
            case mavis::InstMetaData::OperandTypes::SINGLE:
            case mavis::InstMetaData::OperandTypes::DOUBLE:
            case mavis::InstMetaData::OperandTypes::QUAD:
                reg_type = RegType::FLOATING_POINT;
                break;
            case mavis::InstMetaData::OperandTypes::VECTOR:
                reg_type = RegType::VECTOR;
                break;
            case mavis::InstMetaData::OperandTypes::NONE:
                sparta_assert(false, "Invalid Mavis Operand Type!");
        }

        // TODO AtlasRegister: Update this function to use new AtlasState methods for accessing
        // registers
        return state->getAtlasRegister(reg_type, operand->field_value);
    }

    AtlasInst::AtlasInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                         const AtlasExtractorPtr & extractor_info, AtlasState* state) :
        uid_(state->assignUid()),
        opcode_info_(opcode_info),
        extractor_info_(extractor_info),
        opcode_size_(((getOpcode() & 0x3) != 0x3) ? 2 : 4),
        rs1_(getAtlasReg<mavis::InstMetaData::OperandFieldID::RS1>(
            state, opcode_info->getSourceOpInfoList())),
        rs2_(getAtlasReg<mavis::InstMetaData::OperandFieldID::RS2>(
            state, opcode_info->getSourceOpInfoList())),
        rd_(getAtlasReg<mavis::InstMetaData::OperandFieldID::RD>(state,
                                                                 opcode_info->getDestOpInfoList())),
        inst_action_group_(extractor_info_->inst_action_group_)
    {
    }

    std::ostream & operator<<(std::ostream & os, const AtlasInst & inst)
    {
        os << "uid: " << std::dec << inst.uid_ << " " << inst.dasmString() << " "
           << inst.inst_action_group_;
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const AtlasInstPtr & inst)
    {
        os << *inst;
        return os;
    }
} // namespace atlas
