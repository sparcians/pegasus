#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "arch/RegisterSet.hpp"

namespace atlas
{

std::string getSimStatusJson(const AtlasState::SimState* sim_state)
{
    (void)sim_state;
    return "{}";
}

std::string getInstJson(const AtlasInst* insn)
{
    (void)insn;
    return "{}";
}

std::string getRegisterSetJson(const RegisterSet* rset)
{
    (void)rset;
    return "{}";
}

std::string getRegisterJson(const sparta::Register* reg)
{
    (void)reg;
    return "{}";
}

std::string getBreakpointsJson()
{
    return "{}";
}

} // namespace atlas
