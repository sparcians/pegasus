#pragma once

#include "include/PegasusTypes.hpp"
#include "cosim/CoSimApi.hpp"
#include "sparta/utils/ValidValue.hpp"
#include "sparta/serialization/checkpoint/CherryPickFastCheckpointer.hpp"
#include <map>
#include <vector>

namespace sparta
{
    class Scheduler;

    namespace app
    {
        class SimulationConfiguration;
    }
} // namespace sparta

namespace pegasus
{
    class PegasusSim;
    class PegasusState;
} // namespace pegasus

namespace simdb
{
    class DatabaseManager;
} // namespace simdb

namespace pegasus::cosim
{

    class CoSimEventPipeline;
    class Event;

    class CoSimEventReplayer
    {
      public:
        CoSimEventReplayer(const std::string & db_file, const std::string & arch);

        const PegasusSim & getPegasusSim() const;

        PegasusSim & getPegasusSim();

        bool step(CoreId core_id, HartId hart_id);

        const Event* getLastEvent(CoreId core_id, HartId hart_id) const;

      private:
        std::unique_ptr<Event> recreateEventFromDisk_(uint64_t arch_id, CoreId core_id,
                                                      HartId hart_id);

        using CheckpointReplayer = sparta::serialization::checkpoint::CherryPickFastCheckpointer::
            DatabaseCheckpointReplayer;

        template <typename XLEN> static void apply_(const Event & reload_evt, PegasusState* state);

        std::shared_ptr<simdb::DatabaseManager> db_mgr_;
        std::shared_ptr<sparta::Scheduler> scheduler_;
        std::shared_ptr<pegasus::PegasusSim> pegasus_sim_;
        std::shared_ptr<sparta::app::SimulationConfiguration> sim_config_;
        std::vector<std::vector<std::shared_ptr<CheckpointReplayer>>> checkpoint_replayers_;

        uint64_t next_arch_id_ = 0;
        uint64_t num_events_on_disk_ = 0;
        size_t reg_width_ = 0;
        EventList cached_window_;
    };

} // namespace pegasus::cosim
