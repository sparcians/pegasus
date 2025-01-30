#include "core/ActionGroup.hpp"
#include "include/AtlasTypes.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

#include <algorithm>
#include <map>

namespace atlas
{
    class AtlasInst
    {
      public:
        using base_type = AtlasInst;

        AtlasInst(const uint64_t uid, const std::string & mnemonic, const bool is_wfi = false) :
            uid_(uid),
            mnemonic_(mnemonic),
            is_wfi_(is_wfi)
        {
        }

        uint64_t getUid() const { return uid_; }

        const std::string & getMnemonic() const { return mnemonic_; }

        bool isWfi() const { return is_wfi_; }

      private:
        const uint64_t uid_;
        const std::string mnemonic_;
        const bool is_wfi_;
    };

    using AtlasInstPtr = sparta::SpartaSharedPointer<AtlasInst>;

    class AtlasState
    {
      public:
        using base_type = AtlasState;

        AtlasState() { reset(); }

        void incrUid() { ++uid_; }

        uint64_t getUid() const { return uid_; }

        void incrPc() { pc_ += 4; }

        uint64_t getPc() const { return pc_; }

        void setCurrentInst(const AtlasInstPtr & inst_ptr) { current_inst_ptr_ = inst_ptr; }

        const AtlasInstPtr & getCurrentInst() const { return current_inst_ptr_; }

        void incrNumInstsExecuted() { ++num_insts_executed_; }

        uint64_t getNumInstsExecuted() const { return num_insts_executed_; }

        void incrNumActionsExecuted() { ++num_actions_executed_; }

        uint64_t getNumActionsExecuted() const { return num_actions_executed_; }

        atlas::ActionGroup* dummy_action(atlas::AtlasState* state)
        {
            (void)state;
            std::cout << "Executing dummy action" << std::endl;
            ++num_actions_executed_;
            return nullptr;
        }

        atlas::ActionGroup* getStopActionGroup() { return &stop_sim; }

        atlas::ActionGroup* stopSim(AtlasState* state)
        {
            (void)state;
            std::cout << "Num insts executed: " << num_insts_executed_ << std::endl;
            std::cout << "Num actions executed: " << num_actions_executed_ << std::endl;
            std::cout << std::endl;

            // Stop sim
            return nullptr;
        }

        void reset()
        {
            uid_ = 0;
            pc_ = 0x0;
            current_inst_ptr_ = nullptr;
            num_insts_executed_ = 0;
            num_actions_executed_ = 0;
        }

      private:
        uint64_t uid_;
        atlas::Addr pc_;
        AtlasInstPtr current_inst_ptr_;

        atlas::ActionGroup stop_sim{"stop_sim",
                                    atlas::Action::createAction<&atlas::AtlasState::stopSim>(
                                        this, "StopSim", atlas::ActionTags::STOP_SIM_TAG)};

        // For testing
        uint32_t num_insts_executed_;
        uint32_t num_actions_executed_;
    };
} // namespace atlas

std::ostream & operator<<(std::ostream & os, const atlas::AtlasInst & inst)
{
    os << inst.getMnemonic() << " uid:" << std::dec << inst.getUid();
    return os;
}

std::ostream & operator<<(std::ostream & os, const atlas::AtlasInstPtr & inst)
{
    os << *inst;
    return os;
}

// Base Unit class
class Unit
{
  public:
    using base_type = Unit;

    Unit(const std::string name) : name_(name) {}

    virtual ~Unit() {}

    const std::string & getName() const { return name_; }

  private:
    const std::string name_;
};

class FetchUnit : public Unit
{
  public:
    FetchUnit() : Unit("Fetch")
    {
        // Initialize a fake workload
        atlas::Addr pc = 0;
        while (pc < 0x12)
        {
            workload_[pc] = "nop";
            pc += 4;
        }
        workload_[pc] = "wfi";
    }

    atlas::ActionGroup* fetch_inst(atlas::AtlasState* state)
    {
        // Set the current inst, using the PC as an index
        const atlas::Addr pc = state->getPc();
        std::cout << "Fetching PC 0x" << std::hex << pc << std::endl;

        // Read the "opcode" from the "workload"
        const std::string & opcode = workload_.at(pc);

        // Construct and set the current inst
        const bool is_wfi = (opcode == "wfi") ? true : false;
        fetched_insts_.emplace_back(new atlas::AtlasInst(state->getUid(), workload_[pc], is_wfi));
        state->setCurrentInst(fetched_insts_.back());
        state->incrUid();

        state->incrNumActionsExecuted();

        // Always go to either Decode or Translate
        return nullptr;
    }

  private:
    std::map<atlas::Addr, std::string> workload_;
    std::vector<atlas::AtlasInstPtr> fetched_insts_;
};

class TranslateUnit : public Unit
{
  public:
    TranslateUnit() : Unit("Translate") {}

    atlas::ActionGroup* translate_addr(atlas::AtlasState* state)
    {
        // std::cout << "Translating" << std::endl;
        state->incrNumActionsExecuted();

        // Always go to Decode
        return nullptr;
    }
};

class DecodeUnit : public Unit
{
  public:
    DecodeUnit() : Unit("Decode") {}

    atlas::ActionGroup* decode_inst(atlas::AtlasState* state)
    {
        // std::cout << "Decoding" << std::endl;
        state->incrNumActionsExecuted();

        // Always go to Execute
        return nullptr;
    }
};

class ExecuteUnit : public Unit
{
  public:
    ExecuteUnit(atlas::ActionGroup* atlas_core) : Unit("Execute"), atlas_core_(atlas_core) {}

    atlas::ActionGroup* execute_inst(atlas::AtlasState* state)
    {
        // Get current inst
        const atlas::AtlasInstPtr inst = state->getCurrentInst();

        auto inst_action_group_it = inst_action_groups_.find(inst->getMnemonic());
        if (inst_action_group_it == inst_action_groups_.end())
        {
            auto ret = inst_action_groups_.insert({inst->getMnemonic(), {inst->getMnemonic()}});
            sparta_assert(ret.second == true,
                          "Failed to create instruction ActionGroup for " << inst);

            // Create an ActionGroup for the current instruction
            inst_action_group_it = ret.first;
            atlas::ActionGroup & inst_action_group = inst_action_group_it->second;
            inst_action_group.addAction(atlas::Action::createAction<&ExecuteUnit::inst_handler>(
                this, inst->getMnemonic().c_str()));

            if (inst->isWfi())
            {
                inst_action_group.setNextActionGroup(state->getStopActionGroup());
            }
            else
            {
                // Go back to Fetch
                inst_action_group.setNextActionGroup(atlas_core_);
            }
        }

        state->incrNumActionsExecuted();

        // Execute the instruction
        return &inst_action_group_it->second;
    }

    atlas::ActionGroup* inst_handler(atlas::AtlasState* state)
    {
        const atlas::AtlasInstPtr & inst = state->getCurrentInst();
        std::cout << "Executing " << inst << std::endl;

        state->incrNumInstsExecuted();
        state->incrNumActionsExecuted();

        // Increment the PC
        state->incrPc();

        return nullptr;
    }

  private:
    atlas::ActionGroup* atlas_core_;
    std::map<std::string, atlas::ActionGroup> inst_action_groups_;
};
