#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "arch/RegisterSet.hpp"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>

namespace atlas
{

std::string getSimStatusJson(AtlasState::SimState* sim_state)
{
    rapidjson::Document document;
    document.SetObject();

    document.AddMember("workload_exit_code", sim_state->workload_exit_code, document.GetAllocator());
    document.AddMember("test_passed", sim_state->test_passed, document.GetAllocator());
    document.AddMember("sim_stopped", sim_state->sim_stopped, document.GetAllocator());
    document.AddMember("inst_count", rapidjson::Value().SetUint64(sim_state->inst_count), document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string getCurrentInstJson(AtlasState* state)
{
    auto insn = state->getCurrentInst();
    if (!insn)
    {
        return "{}";
    }

    rapidjson::Document document;
    document.SetObject();

    document.AddMember("mnemonic", rapidjson::Value().SetString(insn->getMnemonic().c_str(), insn->getMnemonic().size(), document.GetAllocator()), document.GetAllocator());
    document.AddMember("dasm_string", rapidjson::Value().SetString(insn->dasmString().c_str(), insn->dasmString().size(), document.GetAllocator()), document.GetAllocator());
    document.AddMember("opcode", rapidjson::Value().SetUint(insn->getOpcode()), document.GetAllocator());
    document.AddMember("has_immediate", insn->hasImmediate(), document.GetAllocator());
    document.AddMember("is_memory_inst", insn->isMemoryInst(), document.GetAllocator());
    document.AddMember("opcode_size", rapidjson::Value().SetUint(insn->getOpcodeSize()), document.GetAllocator());
    document.AddMember("priv", rapidjson::Value().SetUint((uint32_t)state->getPrivMode()), document.GetAllocator());

    if (insn->hasImmediate())
    {
        document.AddMember("immediate", rapidjson::Value().SetUint64(insn->getImmediate()), document.GetAllocator());
    }

    if (insn->hasRs1())
    {
        document.AddMember("rs1", rapidjson::Value().SetString(insn->getRs1()->getName().c_str(), insn->getRs1()->getName().size(), document.GetAllocator()), document.GetAllocator());
        document.AddMember("rs1val", rapidjson::Value().SetUint64(insn->getRs1()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    if (insn->hasRs2())
    {
        document.AddMember("rs2", rapidjson::Value().SetString(insn->getRs2()->getName().c_str(), insn->getRs2()->getName().size(), document.GetAllocator()), document.GetAllocator());
        document.AddMember("rs2val", rapidjson::Value().SetUint64(insn->getRs2()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    if (insn->hasRd())
    {
        document.AddMember("rd", rapidjson::Value().SetString(insn->getRd()->getName().c_str(), insn->getRd()->getName().size(), document.GetAllocator()), document.GetAllocator());
        document.AddMember("rdval", rapidjson::Value().SetUint64(insn->getRd()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string getRegisterSetJson(RegisterSet* rset)
{
    (void)rset;
    return "{}";
}

std::string getRegisterJson(sparta::Register* reg)
{
    (void)reg;
    return "{}";
}

std::string getBreakpointsJson()
{
    return "{}";
}

} // namespace atlas
