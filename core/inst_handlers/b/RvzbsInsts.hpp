#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RvzbsInsts
    {
        public:
            using base_type = RvzbsInsts;

            template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);
        
        private:
            template <typename XLEN>
                struct bclr { XLEN operator()(XLEN rs1_val, XLEN rs2_val) const {
                    return rs1_val & ~(XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
                }
            };
            template <typename XLEN>
                struct bext { XLEN operator()(XLEN rs1_val, XLEN rs2_val) const {
                    return (rs1_val >> (rs2_val & (sizeof(XLEN) * 8 - 1))) & XLEN(1);
                }
            };
            template <typename XLEN>
                struct binv { XLEN operator()(XLEN rs1_val, XLEN rs2_val) const {
                    return rs1_val ^ (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
                }
            };
            template <typename XLEN>
                struct bset { XLEN operator()(XLEN rs1_val, XLEN rs2_val) const {
                    return rs1_val | (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
                }
            };
            template <typename XLEN, typename OP>
            Action::ItrType binaryOpHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN, typename OP>
            Action::ItrType immOpHandler(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas