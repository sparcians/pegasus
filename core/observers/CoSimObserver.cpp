#include "core/observers/CoSimObserver.hpp"
#include "core/PegasusCore.hpp"
#include "core/PegasusInst.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "cosim/PegasusCoSim.hpp"
#include "include/PegasusUtils.hpp"
#include "system/PegasusSystem.hpp"
#include "simdb/pipeline/Pipeline.hpp"
#include "sparta/serialization/checkpoint/CherryPickFastCheckpointer.hpp"
#include "source/include/softfloat.h"

namespace pegasus::cosim
{
    CoSimObserver::CoSimObserver(sparta::log::MessageSource & cosim_logger,
                                 CoSimEventPipeline* evt_pipeline, CoSimCheckpointer* checkpointer,
                                 CoreId core_id, HartId hart_id) :
        Observer(ObserverMode::RV64),
        cosim_logger_(cosim_logger),
        evt_pipeline_(evt_pipeline),
        checkpointer_(checkpointer),
        core_id_(core_id),
        hart_id_(hart_id)
    {
        // TODO: CoSimObserver for rv32
    }

    CoSimEventPipeline* CoSimObserver::getEventPipeline() { return evt_pipeline_; }

    const CoSimEventPipeline* CoSimObserver::getEventPipeline() const { return evt_pipeline_; }

    CoSimCheckpointer* CoSimObserver::getCheckpointer() { return checkpointer_; }

    const CoSimCheckpointer* CoSimObserver::getCheckpointer() const { return checkpointer_; }

    void CoSimObserver::preExecute_(PegasusState* state) { resetLastEvent_(state); }

    void CoSimObserver::postExecute_(PegasusState* state)
    {
        auto & last_event = last_event_.getValue();
        for (auto & src_reg : src_regs_)
        {
            last_event.register_reads_.emplace_back(src_reg.reg_id,
                                                    src_reg.reg_value.getByteVector());
        }

        for (auto & dst_reg : dst_regs_)
        {
            last_event.register_writes_.emplace_back(dst_reg.reg_id,
                                                     dst_reg.reg_value.getByteVector(),
                                                     dst_reg.reg_prev_value.getByteVector());
        }

        for (auto & [csr_num, csr_write] : csr_writes_)
        {
            (void)csr_num;
            last_event.register_writes_.emplace_back(csr_write.reg_id,
                                                     csr_write.reg_value.getByteVector(),
                                                     csr_write.reg_prev_value.getByteVector());
        }

        last_event.done_ = true;
        last_event.event_ends_sim_ = state->getSimState()->sim_stopped;
        last_event.sim_state_current_uid_ = state->getSimState()->current_uid;

        if (const auto & reservation = state->getCore()->getReservation(state->getHartId());
            reservation.isValid())
        {
            last_event.end_reservation_ = reservation;
        }

        last_event.end_softfloat_flags_.softfloat_roundingMode = softfloat_roundingMode;
        last_event.end_softfloat_flags_.softfloat_detectTininess = softfloat_detectTininess;
        last_event.end_softfloat_flags_.softfloat_exceptionFlags = softfloat_exceptionFlags;
        last_event.end_softfloat_flags_.extF80_roundingPrecision = extF80_roundingPrecision;

        last_event.next_pc_ = state->getPc();
        last_event.next_priv_ = state->getPrivMode();
        last_event.next_ldst_priv_ = state->getLdstPrivMode();
        sparta_assert(
            last_event.next_pc_ != last_event.curr_pc_,
            "Next PC is the same as the current PC! Check ordering of post-execute Events");
        // TODO: for branches, is_change_of_flow_, alternate_next_pc_

        sendLastEvent_();
    }

    void CoSimObserver::preException_(PegasusState* state) { resetLastEvent_(state); }

    void CoSimObserver::resetLastEvent_(PegasusState* state)
    {
        last_event_ = cosim::Event(cosim::Event::Type::INSTRUCTION);
        auto & last_event = last_event_.getValue();
        last_event.core_id_ = core_id_;
        last_event.hart_id_ = hart_id_;

        if (auto inst = state->getCurrentInst())
        {
            last_event.arch_id_ = inst->getUid();
            last_event.opcode_ = inst->getOpcode();
            last_event.opcode_size_ = inst->getOpcodeSize();
            last_event.dasm_string_ = inst->getMavisOpcodeInfo()->dasmString();
            // TODO: inst_type_
        }
        else
        {
            // No instruction means the fetch unit was not able to decode
            // the current opcode. Set the opcode from the SimState instead.
            last_event.opcode_ = state->getSimState()->current_opcode;
        }

        last_event.curr_pc_ = state->getPc();
        last_event.curr_priv_ = state->getPrivMode();
        last_event.curr_ldst_priv_ = state->getLdstPrivMode();
        last_event.prev_excp_code_ = state->getCurrentException();

        if (const auto & reservation = state->getCore()->getReservation(state->getHartId());
            reservation.isValid())
        {
            last_event.start_reservation_ = reservation;
        }

        last_event.start_softfloat_flags_.softfloat_roundingMode = softfloat_roundingMode;
        last_event.start_softfloat_flags_.softfloat_detectTininess = softfloat_detectTininess;
        last_event.start_softfloat_flags_.softfloat_exceptionFlags = softfloat_exceptionFlags;
        last_event.start_softfloat_flags_.extF80_roundingPrecision = extF80_roundingPrecision;

        const auto & inst = state->getCurrentInst();
        if (inst && inst->hasCsr())
        {
            const uint32_t csr =
                inst->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
            if (state->getCsrRegisterSet()->getRegister(csr))
            {
                last_event.inst_csr_ = csr;
            }
        }
    }

    void CoSimObserver::sendLastEvent_()
    {
        auto & last_event = last_event_.getValue();
        sparta_assert(last_event.isDone(), "Last Event is not done yet!");
        last_event.event_uid_ = checkpointer_->getFastCheckpointer().createCheckpoint();

        COSIMLOG(last_event);
        if (last_event.getRegisterReads().empty() == false)
        {
            COSIMLOG("    " << last_event.getRegisterReads());
        }
        if (last_event.getRegisterWrites().empty() == false)
        {
            COSIMLOG("    " << last_event.getRegisterWrites());
        }

        evt_pipeline_->onStep(std::move(last_event));
        last_event_.clearValid();
    }

    void CoSimObserver::stopSim() { evt_pipeline_->stopSim(); }

} // namespace pegasus::cosim
