#include "core/inst_handlers/v/RvviaInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/VectorState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvviaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // TODO: decide how to pass VLEN as argument.
        using VLEN = std::array<GetUintType<8>, 8>;

        inst_handlers.emplace(
            "vadd.vv", atlas::Action::createAction<&RvviaInsts::viavv_handler_<XLEN, VLEN, std::plus>, RvviaInsts>(
                          nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            atlas::Action::createAction<&RvviaInsts::viavx_handler_<XLEN, VLEN, std::plus>, RvviaInsts>(
                nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            atlas::Action::createAction<&RvviaInsts::viavi_handler_<XLEN, VLEN, std::plus>, RvviaInsts>(
                nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));
    }

    template void RvviaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvviaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename VLEN, typename SEW, typename OP> void viavv_helper(AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        VectorState* vector_state = state->getVectorState();
        uint8_t vlenb = sizeof(VLEN);
        uint8_t sewb = sizeof(SEW);
        VLEN rs1_val, rs2_val, rd_val;
        for (uint8_t index = vector_state->getVSTART(); index < vector_state->getVL(); ++index)
        {
            if (index == vector_state->getVSTART() || index % (vlenb / sewb) == 0)
            {
                rs1_val = READ_VEC_REG<VLEN>(state, inst->getRs1() + index / (vlenb / sewb));
                rs2_val = READ_VEC_REG<VLEN>(state, inst->getRs2() + index / (vlenb / sewb));
                rd_val = READ_VEC_REG<VLEN>(state, inst->getRd() + index / (vlenb / sewb));
            }
            uint8_t result = OP()(
                *(reinterpret_cast<uint8_t*>(rs1_val.data() + (index % (vlenb / sewb)) * sewb)),
                *(reinterpret_cast<uint8_t*>(rs2_val.data() + (index % (vlenb / sewb)) * sewb)));
            memcpy(rd_val.data() + (index % (vlenb / sewb)) * sewb, &result, sewb);
            if (index == vector_state->getVL() - 1 || index % (vlenb / sewb) == (vlenb / sewb) - 1)
            {
                WRITE_VEC_REG<VLEN>(state, inst->getRd() + index / (vlenb / sewb), rd_val);
            }
        }
    }

    template <typename XLEN, typename VLEN, template<typename> typename OP>
    ActionGroup* RvviaInsts::viavv_handler_(AtlasState* state)
    {
        VectorState* vector_state = state->getVectorState();
        switch (vector_state->getSEW())
        {
            case 8:
                viavv_helper<VLEN, uint8_t, OP<uint8_t>>(state);
                break;
            case 16:
                viavv_helper<VLEN, uint16_t, OP<uint16_t>>(state);
                break;
            case 32:
                viavv_helper<VLEN, uint32_t, OP<uint32_t>>(state);
                break;
            case 64:
                viavv_helper<VLEN, uint64_t, OP<uint64_t>>(state);
                break;
            default:
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return nullptr;
    }

    template <typename XLEN, typename VLEN, template<typename> typename OP>
    ActionGroup* RvviaInsts::viavx_handler_(AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    template <typename XLEN, typename VLEN, template<typename> typename OP>
    ActionGroup* RvviaInsts::viavi_handler_(AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

} // namespace atlas
