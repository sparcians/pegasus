#include "core/observers/CoSimObserver.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "cosim/PegasusCoSim.hpp"
#include "include/PegasusUtils.hpp"
#include "system/PegasusSystem.hpp"
#include "simdb/pipeline/Pipeline.hpp"
#include "sparta/serialization/checkpoint/DatabaseCheckpointer.hpp"

namespace pegasus::cosim
{
    CoSimObserver::CoSimObserver(sparta::log::MessageSource & cosim_logger,
                                 CoSimEventPipeline* evt_pipeline,
                                 CoSimCheckpointer* checkpointer,
                                 CoreId core_id,
                                 HartId hart_id) :
        Observer(ObserverMode::RV64),
        cosim_logger_(cosim_logger),
        evt_pipeline_(evt_pipeline),
        checkpointer_(checkpointer),
        core_id_(core_id),
        hart_id_(hart_id)
    {
        // TODO: CoSimObserver for rv32
    }

    CoSimEventPipeline* CoSimObserver::getEventPipeline()
    {
        return evt_pipeline_;
    }

    const CoSimEventPipeline* CoSimObserver::getEventPipeline() const
    {
        return evt_pipeline_;
    }

    CoSimCheckpointer* CoSimObserver::getCheckpointer()
    {
        return checkpointer_;
    }

    const CoSimCheckpointer* CoSimObserver::getCheckpointer() const
    {
        return checkpointer_;
    }

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

        last_event.done_ = true;
        last_event.event_ends_sim_ = state->getSimState()->sim_stopped;

        last_event.next_pc_ = state->getPc();
        sparta_assert(
            last_event.next_pc_ != last_event.curr_pc_,
            "Next PC is the same as the current PC! Check ordering of post-execute Events");
        // TODO: for branches, is_change_of_flow_, alternate_next_pc_
        // TODO: next_priv_

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

        last_event.curr_pc_ = state->getPc();
        // TODO: curr_priv_
    }

    void CoSimObserver::sendLastEvent_()
    {
        auto & last_event = last_event_.getValue();
        sparta_assert(last_event.isDone(), "Last Event is not done yet!");
        last_event.event_uid_ = ++event_uid_;
        last_event.checkpoint_id_ = checkpointer_->createCheckpoint();

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

    void CoSimObserver::loadState_(uint64_t euid, PegasusState* state)
    {
        sparta_assert(euid <= event_uid_,
                      "Cannot load state for event uid " + std::to_string(euid) +
                      " since current event uid is " + std::to_string(event_uid_) + "!");

        checkpointer_->loadCheckpoint(euid);
        event_uid_ = euid;

        // So far, we have reloaded the ArchData state (registers etc.) and now we have
        // to reload the PegasusState state to match (PC, privilege mode, etc.)
        auto reload_evt = evt_pipeline_->getEvent(euid);
        reload_evt->apply_(state);
    }

    void Event::apply_(PegasusState* state) const
    {
        state->setPc(curr_pc_);
        state->setNextPc(next_pc_);
        state->setPrivMode(curr_priv_, state->getVirtualMode());

        auto sim_state = state->getSimState();
        sim_state->reset();
        sim_state->current_opcode = opcode_;
        sim_state->current_uid = event_uid_;
        sim_state->sim_stopped = event_ends_sim_;
        sim_state->inst_count = event_uid_;

        PegasusInstPtr inst = state->getMavis()->makeInst(opcode_, state);
        inst->updateVecConfig(state);
        state->setCurrentInst(inst);
    }

} // namespace pegasus::cosim
