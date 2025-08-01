#include "core/observers/CoSimObserver.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "cosim/CoSimPipeline.hpp"
#include "cosim/PegasusCoSim.hpp"
#include "include/PegasusUtils.hpp"
#include "system/PegasusSystem.hpp"
#include "simdb/pipeline/Pipeline.hpp"

namespace pegasus::cosim
{
    CoSimObserver::CoSimObserver(sparta::log::MessageSource & cosim_logger,
                                 CoSimPipeline* cosim_pipeline, HartId hart_id) :
        Observer(ObserverMode::RV64),
        cosim_logger_(cosim_logger),
        cosim_pipeline_(cosim_pipeline),
        hart_id_(hart_id)
    {
        // TODO: CoSimObserver for rv32
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
        last_event.hart_id_ = state->getHartId();

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

        COSIMLOG(last_event);
        if (last_event.getRegisterReads().empty() == false)
        {
            COSIMLOG("    " << last_event.getRegisterReads());
        }
        if (last_event.getRegisterWrites().empty() == false)
        {
            COSIMLOG("    " << last_event.getRegisterWrites());
        }

        cosim_pipeline_->process(std::move(last_event));
        last_event_.clearValid();
    }

    void CoSimObserver::stopSim() { cosim_pipeline_->stopSim(hart_id_); }
} // namespace pegasus::cosim
