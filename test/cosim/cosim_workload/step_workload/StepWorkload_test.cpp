#include "cosim/PegasusCoSim.hpp"
#include "cosim/CoSimPipeline.hpp"
#include "simdb/apps/AppManager.hpp"
#include "sparta/utils/SpartaTester.hpp"

#include <algorithm>
#include <random>

/// In this test, we will be running a workload one instruction at a time,
/// and getting back EventAccessors each step. The PipelineEventValidator
/// is used to request events from the pipeline, whether they exist in the
/// cache or not, and verify all of them against the original.
class PipelineEventValidator : public pegasus::cosim::CoSimPipelineSnooper
{
  public:
    PipelineEventValidator(pegasus::cosim::CoSimPipeline* cosim_pipeline) :
        cosim_pipeline_(cosim_pipeline)
    {
        cosim_pipeline->setSnooper(this);
    }

    ~PipelineEventValidator()
    {
        // Make sure the test was run long enough to have anything to do
        EXPECT_TRUE(validation_attempted_);
    }

    void onNewEvent(pegasus::cosim::EventAccessor && event) override
    {
        // Sometimes add a new validator
        if (rand() % 5 == 0)
        {
            // Make a deep copy of the event to verify against later
            auto event_truth = *event.get();
            EventValidator event_validator(std::move(event_truth), cosim_pipeline_);
            event_validators_.emplace_back(std::move(event_validator));
        }

        // Sometimes validate all events
        if (rand() % 100 == 0)
        {
            // Shuffle the validators based on their event uids
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(event_validators_.begin(), event_validators_.end(), g);

            // Sleep a bit to get some cache misses
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            for (auto & validator : event_validators_)
            {
                validator.validate();
                validation_attempted_ = true;
            }
        }

        // Sometimes trim the validators
        if (rand() % 50 == 0)
        {
            // Sort by oldest to newest and toss the oldest half
            std::sort(event_validators_.begin(), event_validators_.end(),
                      std::greater<EventValidator>{});

            const size_t target_size = event_validators_.size() / 2;
            while (event_validators_.size() > target_size)
            {
                event_validators_.pop_back();
            }
        }
    }

  private:
    class EventValidator
    {
      public:
        EventValidator(pegasus::cosim::Event && event,
                       pegasus::cosim::CoSimPipeline* cosim_pipeline) :
            event_truth_(std::move(event)),
            cosim_pipeline_(cosim_pipeline)
        {
        }

        uint64_t getEuid() const { return event_truth_.getEuid(); }

        /// Compare euids. Used for std::sort / std::greater.
        bool operator>(const EventValidator & other) const
        {
            return event_truth_.getEuid() > other.event_truth_.getEuid();
        }

        /// Request our event from the pipeline while threads are running,
        /// and verify against the original event given to our constructor.
        void validate()
        {
            auto accessor =
                cosim_pipeline_->getEvent(event_truth_.getEuid(), event_truth_.getHartId());
            auto event = *accessor.get();
            EXPECT_EQUAL(event, event_truth_);
        }

      private:
        pegasus::cosim::Event event_truth_;
        pegasus::cosim::CoSimPipeline* cosim_pipeline_ = nullptr;
    };

    std::vector<EventValidator> event_validators_;
    pegasus::cosim::CoSimPipeline* cosim_pipeline_ = nullptr;
    bool validation_attempted_ = false;
};

void stepCoSimWorkload(const std::string & workload)
{
    const uint64_t ilimit = 0;
    sparta::Scheduler scheduler;
    pegasus::cosim::PegasusCoSim cosim(&scheduler, ilimit, workload);

    const pegasus::HartId hart_id = 0;
    std::string exception_str;

    cosim.enableLogger("step.log");
    cosim.getPegasusState(hart_id)->boot();

    PipelineEventValidator evt_validator(cosim.getCoSimPipeline());

    while (true)
    {
        try
        {
            auto event = cosim.step(hart_id);
            if (event->isLastEvent())
            {
                break;
            }
        }
        catch (const std::exception & ex)
        {
            exception_str = ex.what();
            break;
        }
    }

    cosim.finish();

    auto state = cosim.getPegasusState(hart_id);
    auto sim_state = state->getSimState();
    auto workload_exit_code = sim_state->workload_exit_code;

    EXPECT_EQUAL(workload_exit_code, 0);
    EXPECT_EQUAL(exception_str, "");
}

int main(int argc, char** argv)
{
    const std::string workload = argc >= 2 ? argv[1] : "";
    if (workload.empty())
    {
        std::cout << "Must supply workload!" << std::endl;
        return 1;
    }

    stepCoSimWorkload(workload);

    REPORT_ERROR;
    return ERROR_CODE;
}
