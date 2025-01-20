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

    // Create inner JSON object
    rapidjson::Value response_payload(rapidjson::kObjectType);
    response_payload.AddMember("workload_exit_code", rapidjson::Value().SetUint(sim_state->workload_exit_code), document.GetAllocator());
    response_payload.AddMember("test_passed", rapidjson::Value().SetBool(sim_state->test_passed), document.GetAllocator());
    response_payload.AddMember("sim_stopped", rapidjson::Value().SetBool(sim_state->sim_stopped), document.GetAllocator());
    response_payload.AddMember("inst_count", rapidjson::Value().SetUint(sim_state->inst_count), document.GetAllocator());

    // Add response code, payload, and type to the document
    document.AddMember("response_code", rapidjson::Value().SetString("ok", 2, document.GetAllocator()), document.GetAllocator());
    document.AddMember("response_payload", response_payload, document.GetAllocator());
    document.AddMember("response_type", rapidjson::Value().SetString("simstate", 8, document.GetAllocator()), document.GetAllocator());

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

    // Create inner JSON object
    rapidjson::Value response_payload(rapidjson::kObjectType);

    response_payload.AddMember("uid", rapidjson::Value().SetUint64(insn->getUid()), document.GetAllocator());
    response_payload.AddMember("mnemonic", rapidjson::Value().SetString(insn->getMnemonic().c_str(), insn->getMnemonic().size(), document.GetAllocator()), document.GetAllocator());
    response_payload.AddMember("dasm_string", rapidjson::Value().SetString(insn->dasmString().c_str(), insn->dasmString().size(), document.GetAllocator()), document.GetAllocator());
    response_payload.AddMember("opcode", rapidjson::Value().SetUint(insn->getOpcode()), document.GetAllocator());
    response_payload.AddMember("has_immediate", insn->hasImmediate(), document.GetAllocator());
    response_payload.AddMember("is_memory_inst", insn->isMemoryInst(), document.GetAllocator());
    response_payload.AddMember("opcode_size", rapidjson::Value().SetUint(insn->getOpcodeSize()), document.GetAllocator());
    response_payload.AddMember("priv", rapidjson::Value().SetUint((uint32_t)state->getPrivMode()), document.GetAllocator());

    if (insn->hasImmediate())
    {
        response_payload.AddMember("immediate", rapidjson::Value().SetUint64(insn->getImmediate()), document.GetAllocator());
    }

    if (insn->hasRs1())
    {
        response_payload.AddMember("rs1", rapidjson::Value().SetString(insn->getRs1()->getName().c_str(), insn->getRs1()->getName().size(), document.GetAllocator()), document.GetAllocator());
        response_payload.AddMember("rs1val", rapidjson::Value().SetUint64(insn->getRs1()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    if (insn->hasRs2())
    {
        response_payload.AddMember("rs2", rapidjson::Value().SetString(insn->getRs2()->getName().c_str(), insn->getRs2()->getName().size(), document.GetAllocator()), document.GetAllocator());
        response_payload.AddMember("rs2val", rapidjson::Value().SetUint64(insn->getRs2()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    if (insn->hasRd())
    {
        response_payload.AddMember("rd", rapidjson::Value().SetString(insn->getRd()->getName().c_str(), insn->getRd()->getName().size(), document.GetAllocator()), document.GetAllocator());
        response_payload.AddMember("rdval", rapidjson::Value().SetUint64(insn->getRd()->dmiRead<uint64_t>()), document.GetAllocator());
    }

    // Add response code, payload, and type to the document
    document.AddMember("response_code", rapidjson::Value().SetString("ok", 2, document.GetAllocator()), document.GetAllocator());
    document.AddMember("response_payload", response_payload, document.GetAllocator());
    document.AddMember("response_type", rapidjson::Value().SetString("inst", 4, document.GetAllocator()), document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string getBreakpointsJson()
{
    return "{}";
}

} // namespace atlas
