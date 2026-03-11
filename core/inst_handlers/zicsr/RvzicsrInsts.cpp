#include "core/inst_handlers/inst_helpers.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusCore.hpp"
#include "core/PegasusInst.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"

#include "include/gen/CSRBitMasks32.hpp"

extern "C"
{
#include "source/include/softfloat.h"
}

namespace pegasus
{
    template <typename XLEN>
    void RvzicsrInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "csrrc",
            pegasus::Action::createAction<&RvzicsrInsts::csrrcHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrc", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrci",
            pegasus::Action::createAction<&RvzicsrInsts::csrrciHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrci", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrs",
            pegasus::Action::createAction<&RvzicsrInsts::csrrsHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrs", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrsi",
            pegasus::Action::createAction<&RvzicsrInsts::csrrsiHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrw",
            pegasus::Action::createAction<&RvzicsrInsts::csrrwHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrwi",
            pegasus::Action::createAction<&RvzicsrInsts::csrrwiHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
    }

    template void RvzicsrInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzicsrInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    void RvzicsrInsts::getCsrUpdateActions(InstHandlers::CsrUpdateActionsMap & csrUpdate_actions)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Unprivileged Floating-Point CSRs
        csrUpdate_actions.emplace(
            FCSR,
            pegasus::Action::createAction<&RvzicsrInsts::fcsrUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "fcsrUpdate"));
        csrUpdate_actions.emplace(
            FFLAGS,
            pegasus::Action::createAction<&RvzicsrInsts::fflagsUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "fflagsUpdate"));
        csrUpdate_actions.emplace(
            FRM,
            pegasus::Action::createAction<&RvzicsrInsts::frmUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "frmUpdate"));

        // Supervisor Trap Setup
        csrUpdate_actions.emplace(
            SSTATUS,
            pegasus::Action::createAction<&RvzicsrInsts::sstatusUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "sstatusUpdate"));

        // Supervisor Protection and Translation
        csrUpdate_actions.emplace(
            SATP,
            pegasus::Action::createAction<&RvzicsrInsts::atpUpdateHandler_<
                                              XLEN, translate_types::TranslationStage::SUPERVISOR>,
                                          RvzicsrInsts>(nullptr, "satpUpdate"));

        // Virtual Supervisor Protection and Translation
        csrUpdate_actions.emplace(
            VSATP, pegasus::Action::createAction<
                       &RvzicsrInsts::atpUpdateHandler_<
                           XLEN, translate_types::TranslationStage::VIRTUAL_SUPERVISOR>,
                       RvzicsrInsts>(nullptr, "vsatpUpdate"));

        // Guest Supervisor Protection and Translation
        csrUpdate_actions.emplace(
            HGATP,
            pegasus::Action::createAction<
                &RvzicsrInsts::atpUpdateHandler_<XLEN, translate_types::TranslationStage::GUEST>,
                RvzicsrInsts>(nullptr, "hgatpUpdate"));

        // Machine Trap Setup
        csrUpdate_actions.emplace(
            MSTATUS,
            pegasus::Action::createAction<&RvzicsrInsts::mstatusUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "mstatusUpdate"));
        csrUpdate_actions.emplace(
            MISA,
            pegasus::Action::createAction<&RvzicsrInsts::misaUpdateHandler_<XLEN>, RvzicsrInsts>(
                nullptr, "misaUpdate"));
        csrUpdate_actions.emplace(
            MTVEC, pegasus::Action::createAction<&RvzicsrInsts::tvecUpdateHandler_<XLEN, MTVEC>,
                                                 RvzicsrInsts>(nullptr, "mtvecUpdate"));
        csrUpdate_actions.emplace(
            STVEC, pegasus::Action::createAction<&RvzicsrInsts::tvecUpdateHandler_<XLEN, STVEC>,
                                                 RvzicsrInsts>(nullptr, "stvecUpdate"));
        csrUpdate_actions.emplace(
            VSTVEC, pegasus::Action::createAction<&RvzicsrInsts::tvecUpdateHandler_<XLEN, VSTVEC>,
                                                  RvzicsrInsts>(nullptr, "vstvecUpdate"));
    }

    template void RvzicsrInsts::getCsrUpdateActions<RV32>(InstHandlers::CsrUpdateActionsMap &);
    template void RvzicsrInsts::getCsrUpdateActions<RV64>(InstHandlers::CsrUpdateActionsMap &);

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrcHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const auto rs1 = inst->getRs1();
        auto rd = inst->getRd();
        const uint32_t csr = getVirtualCsrNum_(state, inst->getCsr());

        if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        // Don't write CSR is rs1=x0
        if (rs1 != 0)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            // rs1 value is treated as a bit mask to clear bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (~rs1_val & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrciHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN imm = inst->getImmediate();
        const auto rd = inst->getRd();
        const int csr = getVirtualCsrNum_(state, inst->getCsr());

        if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            WRITE_CSR_REG<XLEN>(state, csr, (~imm & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrsHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const auto rs1 = inst->getRs1();
        const auto rd = inst->getRd();
        const int csr = getVirtualCsrNum_(state, inst->getCsr());

        if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (rs1 != 0)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            // rs1 value is treated as a bit mask to set bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (rs1_val | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrsiHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN imm = inst->getImmediate();
        const auto rd = inst->getRd();
        const int csr = getVirtualCsrNum_(state, inst->getCsr());

        if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            // imm value is treated as a bit mask
            WRITE_CSR_REG<XLEN>(state, csr, (imm | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrwHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const auto rs1 = inst->getRs1();
        const auto rd = inst->getRd();
        const int csr = getVirtualCsrNum_(state, inst->getCsr());

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);

        if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, rs1_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::csrrwiHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN imm = inst->getImmediate();
        const auto rd = inst->getRd();
        const int csr = getVirtualCsrNum_(state, inst->getCsr());

        if (!isAccessLegal_<RvCsrAccess::AccessType::WRITE>(state, csr))
        {
            THROW_ILLEGAL_INST;
        }

        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            if (!isAccessLegal_<RvCsrAccess::AccessType::READ>(state, csr))
            {
                THROW_ILLEGAL_INST;
            }
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, imm);

        return ++action_it;
    }

    template <typename XLEN> void set_softfloat_excpetionFlags(pegasus::PegasusState* state)
    {
        XLEN softfloat_exceptionFlags_mask = softfloat_flag_inexact | softfloat_flag_underflow
                                             | softfloat_flag_overflow | softfloat_flag_infinite
                                             | softfloat_flag_invalid;
        softfloat_exceptionFlags =
            READ_CSR_REG<XLEN>(state, FFLAGS) & softfloat_exceptionFlags_mask;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::fcsrUpdateHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        // FFLAGS
        const XLEN nx_val = READ_CSR_FIELD<XLEN>(state, FCSR, "NX");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "NX", nx_val);

        const XLEN uf_val = READ_CSR_FIELD<XLEN>(state, FCSR, "UF");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "UF", uf_val);

        const XLEN of_val = READ_CSR_FIELD<XLEN>(state, FCSR, "OF");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "OF", of_val);

        const XLEN dz_val = READ_CSR_FIELD<XLEN>(state, FCSR, "DZ");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "DZ", dz_val);

        const XLEN nv_val = READ_CSR_FIELD<XLEN>(state, FCSR, "NV");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "NV", nv_val);

        // FRM
        const XLEN frm_val = READ_CSR_FIELD<XLEN>(state, FCSR, "frm");
        WRITE_CSR_REG<XLEN>(state, FRM, frm_val);

        set_softfloat_excpetionFlags<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::fflagsUpdateHandler_(PegasusState* state,
                                                       Action::ItrType action_it)
    {
        // FCSR
        const XLEN nx_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "NX");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "NX", nx_val);

        const XLEN uf_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "UF");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "UF", uf_val);

        const XLEN of_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "OF");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "OF", of_val);

        const XLEN dz_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "DZ");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "DZ", dz_val);

        const XLEN nv_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "NV");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "NV", nv_val);

        set_softfloat_excpetionFlags<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::frmUpdateHandler_(PegasusState* state, Action::ItrType action_it)
    {
        // FCSR
        const XLEN frm_val = READ_CSR_REG<XLEN>(state, FRM);
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "frm", frm_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::sstatusUpdateHandler_(pegasus::PegasusState* state,
                                                        Action::ItrType action_it)
    {
        // Update shared fields only
        const XLEN sie_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sie", sie_val);

        const XLEN spie_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "spie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spie", spie_val);

        const XLEN ube_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "ube");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "ube", ube_val);

        const XLEN spp_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "spp");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spp", spp_val);

        const XLEN vs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "vs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "vs", vs_val);

        const XLEN fs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "fs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "fs", fs_val);

        const XLEN xs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "xs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "xs", xs_val);

        const XLEN sum_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sum");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sum", sum_val);

        const XLEN mxr_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "mxr");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mxr", mxr_val);

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            const XLEN uxl_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "uxl");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "uxl", uxl_val);
        }

        const XLEN sd_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sd");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sd", sd_val);

        return mstatusUpdateHandler_<XLEN>(state, action_it);
    }

    template <typename XLEN, translate_types::TranslationStage TYPE>
    Action::ItrType RvzicsrInsts::atpUpdateHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        state->updateTranslationMode<XLEN>(TYPE);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::mstatusUpdateHandler_(pegasus::PegasusState* state,
                                                        Action::ItrType action_it)
    {
        // Non-shared fields of SSTATUS are read-only so writing the MSTATUS value to SSTATUS will
        // only write to the shared fields
        const XLEN mstatus_val = READ_CSR_REG<XLEN>(state, MSTATUS);
        WRITE_CSR_REG<XLEN>(state, SSTATUS, mstatus_val);

        // Update the summarize field
        XLEN mstatus_fast_check_mask = 0;
        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            mstatus_fast_check_mask = MSTATUS_32_bitmasks::XS | MSTATUS_32_bitmasks::FS;
        }
        else
        {
            mstatus_fast_check_mask = MSTATUS_64_bitmasks::XS | MSTATUS_64_bitmasks::FS;
        }

        if (mstatus_val & mstatus_fast_check_mask)
        {
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sd", 0b1);
        }

        auto & ext_manager = state->getCore()->getExtensionManager();
        bool change_mavis_ctx = false;

        // If FS is set to 0 (off), all floating point extensions are disabled
        const uint32_t fs_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "fs");
        if (fs_val == 0)
        {
            std::vector<std::string> disabled_exts;
            if (ext_manager.isEnabled("f"))
            {
                disabled_exts.emplace_back("f");
            }
            if (ext_manager.isEnabled("d"))
            {
                disabled_exts.emplace_back("d");
            }
            ext_manager.disableExtensions(disabled_exts);
            change_mavis_ctx = !disabled_exts.empty();
        }
        else
        {
            std::vector<std::string> enabled_exts;
            if (READ_CSR_FIELD<XLEN>(state, MISA, "f") == 0x1)
            {
                if (!ext_manager.isEnabled("f"))
                {
                    enabled_exts.emplace_back("f");
                }
            }
            if (READ_CSR_FIELD<XLEN>(state, MISA, "d") == 0x1)
            {
                if (!ext_manager.isEnabled("d"))
                {
                    enabled_exts.emplace_back("d");
                }
            }
            ext_manager.enableExtensions(enabled_exts);
            change_mavis_ctx = !enabled_exts.empty();
        }

        if (change_mavis_ctx)
        {
            state->getCore()->changeMavisContext();
        }

        // Updating the MPRV field can affect all translation stages
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::SUPERVISOR);
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::GUEST);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzicsrInsts::misaUpdateHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        const XLEN misa_val = READ_CSR_REG<XLEN>(state, MISA);
        auto & ext_manager = state->getCore()->getExtensionManager();

        std::vector<std::string> exts_to_enable;
        std::vector<std::string> exts_to_disable;
        for (char ext = 'a'; ext <= 'z'; ++ext)
        {
            const std::string ext_str = std::string(1, ext);
            if (ext_manager.isExtensionSupported(ext_str))
            {
                // G bit is reserved
                if (ext == 'g')
                {
                    continue;
                }

                if (misa_val & (1 << (ext - 'a')))
                {
                    exts_to_enable.emplace_back(ext_str);
                }
                else
                {
                    // Disabling compression will fail if doing so would cause an instruction
                    // misalignment exception. Check if the next PC is not 32-bit aligned.
                    if ((ext == 'c') && ((state->getNextPc() & 0x3) != 0))
                    {
                        WRITE_CSR_FIELD<XLEN>(state, MISA, "c", 0x1);
                    }
                    else
                    {
                        exts_to_disable.emplace_back(ext_str);
                    }
                }
            }
        }

        // Remove from exts_to_disable any extensions that are already disabled
        auto it = exts_to_disable.begin();
        while (it != exts_to_disable.end())
        {
            if (!ext_manager.isEnabled(*it))
            {
                it = exts_to_disable.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Remove from exts_to_enable any extensions that are already enabled
        it = exts_to_enable.begin();
        while (it != exts_to_enable.end())
        {
            if (ext_manager.isEnabled(*it))
            {
                it = exts_to_enable.erase(it);
            }
            else
            {
                ++it;
            }
        }

        ext_manager.disableExtensions(exts_to_disable);
        ext_manager.enableExtensions(exts_to_enable);
        state->getCore()->changeMavisContext();

        return ++action_it;
    }

    template <typename XLEN, uint32_t TVEC_CSR_ADDR>
    Action::ItrType RvzicsrInsts::tvecUpdateHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        const TrapVectorMode mode_val =
            (TrapVectorMode)READ_CSR_FIELD<XLEN>(state, TVEC_CSR_ADDR, "mode");
        if (!state->getCore()->isTrapModeSupported(mode_val))
        {
            WRITE_CSR_FIELD<XLEN>(state, TVEC_CSR_ADDR, "mode", 0);
        }

        return ++action_it;
    }
} // namespace pegasus
