#pragma once

#include "core/Action.hpp"
#include "mavis/OpcodeInfo.h"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    class AtlasState;
    class ActionGroup;

    class Observer
    {
      public:
        Observer() { reset_(); }

        virtual ~Observer() = default;

        virtual bool enabled() const { return enabled_; }

        struct SrcReg
        {
            SrcReg(const RegId id) : reg_id(id) {}

            SrcReg(const RegId id, std::vector<uint8_t> value) : reg_id(id), reg_value(value) {}

            void setValue(const std::vector<uint8_t> & value) { reg_value = value; }

            const RegId reg_id;
            std::vector<uint8_t> reg_value;
        };

        struct DestReg : SrcReg
        {
            DestReg(const RegId id, std::vector<uint8_t> prev_value) :
                SrcReg(id),
                reg_prev_value(prev_value)
            {
            }

            void setPrevValue(const std::vector<uint8_t> & value) { reg_prev_value = value; }

            std::vector<uint8_t> reg_prev_value;
        };

        void insertExecuteActions(ActionGroup* action_group);

        uint64_t getPrevRdValue() const
        {
            sparta_assert(dst_regs_.size() == 1);
            return convertFromByteVector<uint64_t>(dst_regs_[0].reg_prev_value);
        }

        virtual void stopSim() {}

      protected:
        bool enabled_ = false;

        uint64_t pc_;
        uint64_t opcode_;

        // Mavis pointer for getting the disassembly string
        mavis::OpcodeInfo::PtrType opcode_info_;

        std::vector<SrcReg> src_regs_;
        std::vector<DestReg> dst_regs_;

        virtual void reset_()
        {
            pc_ = 0;
            opcode_ = 0;
            src_regs_.clear();
            dst_regs_.clear();
        }

        Action pre_execute_action_;
        Action post_execute_action_;
    };
} // namespace atlas
