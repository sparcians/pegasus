#include "core/observers/SimController.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "sparta/trigger/Comparator.hpp"
#include "simdb/utils/uuids.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 65432

namespace atlas
{

class BreakpointManager
{
public:
    void breakPC(uint64_t pc, const std::string& comparison = "==")
    {
        breakpoints_[getNextBreakpointId_()] = std::make_unique<PcBreakpoint>(pc, comparison);
    }

    void breakInst(const std::string& mnemonic)
    {
        breakpoints_[getNextBreakpointId_()] = std::make_unique<InstBreakpoint>(mnemonic);
    }

    void breakException()
    {
        breakpoints_[getNextBreakpointId_()] = std::make_unique<ExceptionBreakpoint>();
    }

    void breakOnPreExecute()
    {
        break_on_pre_execute_ = true;
    }

    void breakOnPostExecute()
    {
        break_on_post_execute_ = true;
    }

    void breakOnPreException()
    {
        break_on_pre_exception_ = true;
    }

    bool shouldBreakOnPreExecute(AtlasState* state) const
    {
        if (break_on_pre_execute_) {
            return true;
        }

        for (const auto& [id, breakpoint] : breakpoints_) {
            if (breakpoint->shouldBreakOnPreExecute(state)) {
                return true;
            }
        }
        return false;
    }

    bool shouldBreakOnPostExecute(AtlasState* state) const
    {
        if (break_on_post_execute_) {
            return true;
        }

        for (const auto& [id, breakpoint] : breakpoints_) {
            if (breakpoint->shouldBreakOnPostExecute(state)) {
                return true;
            }
        }
        return false;
    }

    bool shouldBreakOnPreException(AtlasState* state) const
    {
        if (break_on_pre_exception_) {
            return true;
        }

        for (const auto& [id, breakpoint] : breakpoints_) {
            if (breakpoint->shouldBreakOnPreException(state)) {
                return true;
            }
        }
        return false;
    }

    bool deleteBreakpoint(const size_t id)
    {
        return breakpoints_.erase(id) == 1;
    }

    void deleteAllBreakpoints()
    {
        breakpoints_.clear();
    }

    bool disableBreakpoint(const size_t id)
    {
        if (const auto& breakpoint = breakpoints_.find(id); breakpoint != breakpoints_.end()) {
            breakpoint->second->disable();
            return true;
        }
        return false;
    }

    void disableAllBreakpoints()
    {
        for (const auto& [id, breakpoint] : breakpoints_) {
            breakpoint->disable();
        }
    }

    bool enableBreakpoint(const size_t id)
    {
        if (const auto& breakpoint = breakpoints_.find(id); breakpoint != breakpoints_.end()) {
            breakpoint->second->enable();
            return true;
        }
        return false;
    }

    void enableAllBreakpoints()
    {
        for (const auto& [id, breakpoint] : breakpoints_) {
            breakpoint->enable();
        }
    }

    std::vector<std::pair<size_t, std::string>> getBreakpoints() const
    {
        std::vector<std::pair<size_t, std::string>> result;
        for (const auto& [id, breakpoint] : breakpoints_) {
            result.emplace_back(id, breakpoint->getInfo());
        }
        return result;
    }

private:
    size_t getNextBreakpointId_()
    {
        static size_t id = 1;
        return id++;
    }

    class Breakpoint
    {
    public:
        virtual ~Breakpoint() = default;
        const std::string& getInfo() const { return info_; }
        void enable() { enabled_ = true; }
        void disable() { enabled_ = false; }

        bool shouldBreakOnPreExecute(AtlasState* state) const
        {
            return enabled_ && shouldBreakOnPreExecute_(state);
        }

        bool shouldBreakOnPostExecute(AtlasState* state) const
        {
            return enabled_ && shouldBreakOnPostExecute_(state);
        }

        bool shouldBreakOnPreException(AtlasState* state) const
        {
            return enabled_ && shouldBreakOnPreException_(state);
        }

    protected:
        Breakpoint(const std::string& info)
            : info_(info)
        {}

    private:
        virtual bool shouldBreakOnPreExecute_(AtlasState* state) const = 0;
        virtual bool shouldBreakOnPostExecute_(AtlasState* state) const = 0;
        virtual bool shouldBreakOnPreException_(AtlasState* state) const = 0;

        std::string info_;
        bool enabled_ = true;
    };

    class PcBreakpoint : public Breakpoint
    {
    public:
        PcBreakpoint(uint64_t pc, const std::string& comparison)
            : Breakpoint("Break on PC " + comparison + " " + std::to_string(pc))
            , comparator_(sparta::trigger::createComparator<uint64_t>(comparison, pc))
        {}

        bool shouldBreakOnPreExecute_(AtlasState* state) const override
        {
            return shouldBreakNow(state);
        }

        bool shouldBreakOnPostExecute_(AtlasState* state) const override
        {
            return shouldBreakNow(state);
        }

        bool shouldBreakOnPreException_(AtlasState* state) const override
        {
            return shouldBreakNow(state);
        }

        bool shouldBreakNow(AtlasState* state) const
        {
            return comparator_->eval(state->getPc());
        }

    private:
        std::unique_ptr<sparta::trigger::ComparatorBase<uint64_t>> comparator_;
    };

    class InstBreakpoint : public Breakpoint
    {
    public:
        InstBreakpoint(const std::string& mnemonic)
            : Breakpoint("Break on instruction '" + mnemonic + "'")
            , mnemonic_(mnemonic)
        {}

        bool shouldBreakOnPreExecute_(AtlasState* state) const override
        {
            return shouldBreakNow(state);
        }

        bool shouldBreakOnPostExecute_(AtlasState* state) const override
        {
            return shouldBreakNow(state);
        }

        bool shouldBreakOnPreException_(AtlasState* state) const override
        {
            return false;
        }

        bool shouldBreakNow(AtlasState* state) const
        {
            return state->getCurrentInst()->getMnemonic() == mnemonic_;
        }

    private:
        std::string mnemonic_;
    };

    class ExceptionBreakpoint : public Breakpoint
    {
    public:
        ExceptionBreakpoint()
            : Breakpoint("Break on exceptions")
        {}

        bool shouldBreakOnPreExecute_(AtlasState* state) const override
        {
            return false;
        }

        bool shouldBreakOnPostExecute_(AtlasState* state) const override
        {
            return false;
        }

        bool shouldBreakOnPreException_(AtlasState* state) const override
        {
            return true;
        }
    };

    std::map<size_t, std::unique_ptr<Breakpoint>> breakpoints_;
    bool break_on_pre_execute_ = false;
    bool break_on_post_execute_ = false;
    bool break_on_pre_exception_ = false;
};

class SimController::SimEndpoint
{
public:
    SimEndpoint(const std::string& host = "localhost", int port = PORT)
        : host_(host)
        , port_(port)
        , server_fd_(-1)
        , new_socket_(-1)
    {}

    ~SimEndpoint()
    {
        closeEndpoint();
    }

    void openEndpoint()
    {
        // Create the server socket
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);

        // Bind the socket to the address
        if (bind(server_fd_, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(server_fd_, 3) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Waiting for connection...\n";

        // Accept an incoming connection
        new_socket_ = accept(server_fd_, nullptr, nullptr);
        if (new_socket_ < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
    }

    void postInit(AtlasState* state)
    {
        enterLoop_(state);
    }

    ActionGroup* preExecute(AtlasState* state)
    {
        if (breakpoint_mgr_.shouldBreakOnPreExecute(state)) {
            sendResponse_("pre_execute");
            return enterLoop_(state);
        }
        return nullptr;
    }

    ActionGroup* postExecute(AtlasState* state)
    {
        if (breakpoint_mgr_.shouldBreakOnPostExecute(state)) {
            sendResponse_("post_execute");
            return enterLoop_(state);
        }
        return nullptr;
    }

    ActionGroup* preException(AtlasState* state)
    {
        if (breakpoint_mgr_.shouldBreakOnPreException(state)) {
            sendResponse_("pre_exception");
            return enterLoop_(state);
        }
        return nullptr;
    }

    void onSimulationFinished(AtlasState* state)
    {
        sendResponse_("sim_finished");
        enterLoop_(state);
        closeEndpoint();
    }

    void closeEndpoint()
    {
        if (new_socket_ != -1) {
            close(new_socket_);
            new_socket_ = -1;
        }
        if (server_fd_ != -1) {
            close(server_fd_);
            server_fd_ = -1;
        }
    }

private:
    enum class SimCommand
    {
        STATUS,
        INST,
        DUMP,
        EDIT,
        READ,
        WRITE,
        DMIWRITE,
        BREAKPOINT,
        CONT,
        FINISH,
        EXIT,
        NOP
    };

    SimCommand getSimCommand_(const std::string& request, std::vector<std::string>& args) const
    {
        // Use an istringstream to split the string
        std::istringstream stream(request);

        // Extract words (tokens) one by one and push them to the vector
        std::string command_str;
        std::string word;
        while (stream >> word) {
            if (command_str.empty()) {
                command_str = word;
            } else {
                args.push_back(word);
            }
        }

        if (command_str == "status") {
            return SimCommand::STATUS;
        } else if (command_str == "inst") {
            return SimCommand::INST;
        } else if (command_str == "dump") {
            return SimCommand::DUMP;
        } else if (command_str == "edit") {
            return SimCommand::EDIT;
        } else if (command_str == "read") {
            return SimCommand::READ;
        } else if (command_str == "write") {
            return SimCommand::WRITE;
        } else if (command_str == "dmiwrite") {
            return SimCommand::DMIWRITE;
        } else if (command_str == "break") {
            return SimCommand::BREAKPOINT;
        } else if (command_str == "cont") {
            return SimCommand::CONT;
        } else if (command_str == "finish") {
            return SimCommand::FINISH;
        } else if (command_str == "exit") {
            return SimCommand::EXIT;
        } else {
            return SimCommand::NOP;
        }
    }

    void sendResponse_(const std::string& message)
    {
        send(new_socket_, message.c_str(), message.length(), 0);
    }

    std::string receiveMessage_()
    {
        char buffer[1024] = {0};
        int valread = read(new_socket_, buffer, sizeof(buffer));
        if (valread > 0) {
            return std::string(buffer, valread);
        }
        return "";
    }

    ActionGroup* enterLoop_(AtlasState* state)
    {
        while (true) {
            const std::string request = receiveMessage_();
            std::vector<std::string> args;

            switch (getSimCommand_(request, args)) {
                case SimCommand::STATUS:
                    handleStatusRequest_(state, args);
                    break;
                case SimCommand::INST:
                    handleCurrentInstRequest_(state, args);
                    break;
                case SimCommand::DUMP:
                    handleRegisterDumpRequest_(state, args);
                    break;
                case SimCommand::EDIT:
                    handleEditInstRequest_(state, args);
                    break;
                case SimCommand::READ:
                    handleRegisterReadRequest_(state, args);
                    break;
                case SimCommand::WRITE:
                    handleRegisterWriteRequest_(state, args);
                    break;
                case SimCommand::DMIWRITE:
                    handleRegisterDmiWriteRequest_(state, args);
                    break;
                case SimCommand::BREAKPOINT:
                    handleBreakpointRequest_(state, args);
                    break;
                case SimCommand::CONT:
                    return nullptr;
                case SimCommand::FINISH:
                    handleFinishRequest_(state, args);
                    return nullptr;
                case SimCommand::EXIT:
                    handleExitRequest_(state, args);
                    return nullptr;
                case SimCommand::NOP:
                    sendResponse_("ERROR: '" + request + "' is not a valid command");
                    break;
            }
        }
    }

    void handleStatusRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        if (!args.empty()) {
            sendResponse_("ERROR: 'status' request expects zero arguments");
            return;
        }

        const auto sim_state = state->getSimState();

        std::string json;
        json += "{\n";
        json += "    \"inst_count\": " + std::to_string(sim_state->inst_count) + ",\n";
        json += "    \"sim_stopped\": ";
        if (sim_state->sim_stopped) {
            json += "true,\n";
            json += "    \"test_passed\": ";
            json += sim_state->test_passed ? "true,\n" : "false,\n";
            json += "    \"workload_exit_code\": " + std::to_string(sim_state->workload_exit_code) + "\n";
        } else {
            json += "false\n";
        }
        json += "}\n";

        sendResponse_(json);
    }

    void handleCurrentInstRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        if (!args.empty()) {
            sendResponse_("ERROR: 'inst' request expects zero arguments");
            return;
        }

        const AtlasInstPtr insn = state->getCurrentInst();
        if (!insn) {
            sendResponse_("WARNING: no current instruction");
            return;
        }

        std::string json;
        json += "{\n";
        json += "    \"mnemonic\": \"" + insn->getMnemonic() + "\",\n";
        json += "    \"dasm\": \"" + insn->dasmString() + "\",\n";
        json += "    \"opcode\": " + std::to_string(insn->getOpcode()) + ",\n";
        json += "    \"is_memory_inst\": ";
        json += insn->isMemoryInst() ? "true,\n" : "false,\n";
        json += "    \"opcode_size\": " + std::to_string(insn->getOpcodeSize()) + ",\n";

        if (insn->hasRs1()) {
            json += "    \"rs1\": \"" + insn->getRs1()->getName() + "\",\n";
        }

        if (insn->hasRs2()) {
            json += "    \"rs2\": \"" + insn->getRs2()->getName() + "\",\n";
        }

        if (insn->hasImmediate()) {
            json += "    \"imm\": " + std::to_string(insn->getImmediate()) + ",\n";
            switch (insn->getMavisOpcodeInfo()->getImmediateType()) {
                case mavis::ImmediateType::NONE:
                    json += "    \"imm_type\": \"NONE\"\n";
                    break;
                case mavis::ImmediateType::SIGNED:
                    json += "    \"imm_type\": \"SIGNED\"\n";
                    break;
                case mavis::ImmediateType::UNSIGNED:
                    json += "    \"imm_type\": \"UNSIGNED\"\n";
                    break;
            }
        }

        json += "}\n";
        sendResponse_(json);
    }

    void handleRegisterDumpRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        // dump <group num>
        if (args.size() == 1) {
            const auto group_num = std::stoi(args[0]);

            atlas::RegisterSet* rset = nullptr;
            switch (group_num) {
                case 0:
                    rset = state->getIntRegisterSet();
                    break;
                case 1:
                    rset = state->getFpRegisterSet();
                    break;
                case 2:
                    rset = state->getVecRegisterSet();
                    break;
                case 3:
                    rset = state->getCsrRegisterSet();
                    break;
                default:
                    sendResponse_("ERROR: invalid group number " + std::string(args[0]));
                    return;
            }

            std::string json;
            json += "[\n";
            for (uint32_t reg_id = 0; reg_id < rset->getNumRegisters(); ++reg_id) {
                if (const auto reg = rset->getRegister(reg_id)) {
                    json += "    [\"" + reg->getName() + "\", " + std::to_string(reg_id) + ", " + std::to_string(reg->dmiRead<uint64_t>()) + "]";
                    if (reg_id < rset->getNumRegisters() - 1) {
                        json += ",";
                    }
                    json += "\n";
                }
            }

            json += "]\n";

            // This JSON is too large to send back over the socket without
            // requiring changes to the python code. As a workaround, we
            // will just create a random file in the tmpdir and write the
            // JSON there, and just return the path to the file.
            const auto fname = "/tmp/" + simdb::generateUUID();
            std::ofstream file(fname);
            file << json;
            file.close();

            sendResponse_(fname);
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'dump <group num>'");
        }
    }

    void handleEditInstRequest_(AtlasState*, const std::vector<std::string>& args)
    {
        // edit <inst> [true|false]
        if (args.size() <= 2) {
            const auto& inst = args[0];
            bool editable;
            if (args.size() == 1) {
                editable = true;
            } else {
                if (args[1] == "true") {
                    editable = true;
                } else if (args[1] == "false") {
                    editable = false;
                } else {
                    sendResponse_("ERROR: invoke command as 'edit <inst> <true|false>'");
                    return;
                }
            }

            if (editable) {
                insts_being_edited_.insert(inst);
            } else {
                insts_being_edited_.erase(inst);
            }

            sendResponse_("OK");
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'edit <inst> [true|false]'");
        }
    }

    void handleRegisterReadRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        // read <group num> <reg id>
        if (args.size() == 2) {
            const auto group_num = std::stoi(args[0]);
            const auto reg_id = std::stoi(args[1]);

            atlas::RegisterSet* rset = nullptr;
            switch (group_num) {
                case 0:
                    rset = state->getIntRegisterSet();
                    break;
                case 1:
                    rset = state->getFpRegisterSet();
                    break;
                case 2:
                    rset = state->getVecRegisterSet();
                    break;
                case 3:
                    rset = state->getCsrRegisterSet();
                    break;
                default:
                    sendResponse_("ERROR: invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                sendResponse_(std::to_string(reg->dmiRead<uint64_t>()));
            } else {
                sendResponse_("ERROR: invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'read <group num> <reg id>'");
        }
    }

    void handleRegisterWriteRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        // write <group num> <reg id> <value>
        if (args.size() == 3) {
            const auto group_num = std::stoi(args[0]);
            const auto reg_id = std::stoi(args[1]);
            const auto value = std::stoull(args[2]);

            atlas::RegisterSet* rset = nullptr;
            switch (group_num) {
                case 0:
                    rset = state->getIntRegisterSet();
                    break;
                case 1:
                    rset = state->getFpRegisterSet();
                    break;
                case 2:
                    rset = state->getVecRegisterSet();
                    break;
                case 3:
                    rset = state->getCsrRegisterSet();
                    break;
                default:
                    sendResponse_("ERROR: invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                reg->write(value);
                sendResponse_(std::to_string(reg->dmiRead<uint64_t>()));
            } else {
                sendResponse_("ERROR: invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'write <group num> <reg id> <value>'");
        }
    }

    void handleRegisterDmiWriteRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        // dmiwrite <group num> <reg id> <value>
        if (args.size() == 3) {
            const auto group_num = std::stoi(args[0]);
            const auto reg_id = std::stoi(args[1]);
            const auto value = std::stoull(args[2]);

            atlas::RegisterSet* rset = nullptr;
            switch (group_num) {
                case 0:
                    rset = state->getIntRegisterSet();
                    break;
                case 1:
                    rset = state->getFpRegisterSet();
                    break;
                case 2:
                    rset = state->getVecRegisterSet();
                    break;
                case 3:
                    rset = state->getCsrRegisterSet();
                    break;
                default:
                    sendResponse_("ERROR: invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                reg->dmiWrite(value);
                sendResponse_(std::to_string(reg->dmiRead<uint64_t>()));
            } else {
                sendResponse_("ERROR: invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'write <group num> <reg id> <value>'");
        }
    }

    void handleBreakpointRequest_(AtlasState*, const std::vector<std::string>& args)
    {
        if (args.size() == 1) {
            // "break info"
            if (args[0] == "info") {
                auto breakpoints = breakpoint_mgr_.getBreakpoints();
                std::string json;
                json += "{\n";
                for (const auto& [id, info] : breakpoints) {
                    json += "    " + std::to_string(id) + ": \"" + info + "\"";
                    if (id < breakpoints.size()) {
                        json += ",";
                    }
                    json += "\n";
                }

                json += "}\n";
                sendResponse_(json);
            }

            // "break delete"
            else if (args[0] == "delete") {
                breakpoint_mgr_.deleteAllBreakpoints();
                sendResponse_("OK");
            }

            // "break exception"
            else if (args[0] == "exception") {
                breakpoint_mgr_.breakException();
                sendResponse_("OK");
            }

            // "break disable"
            else if (args[0] == "disable") {
                breakpoint_mgr_.disableAllBreakpoints();
                sendResponse_("OK");
            }

            // "break enable"
            else if (args[0] == "enable") {
                breakpoint_mgr_.enableAllBreakpoints();
                sendResponse_("OK");
            }

            // "break <inst>"
            else {
                breakpoint_mgr_.breakInst(args[0]);
                sendResponse_("OK");
            }
        }

        else if (args.size() == 2) {
            // "break delete [n]"
            if (args[0] == "delete") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.deleteBreakpoint(id)) {
                    sendResponse_("ERROR: invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendResponse_("OK");
                }
            }

            // "break pc [pc]"
            else if (args[0] == "pc") {
                breakpoint_mgr_.breakPC(std::stoull(args[1]));
                sendResponse_("OK");
            }

            // "break disable <n>"
            else if (args[0] == "disable") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.disableBreakpoint(id)) {
                    sendResponse_("ERROR: invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendResponse_("OK");
                }
            }

            // "break enable <n>"
            else if (args[0] == "enable") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.enableBreakpoint(id)) {
                    sendResponse_("ERROR: invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendResponse_("OK");
                }
            }

            // "break action <action>"
            else if (args[0] == "action") {
                const auto& action = args[1];
                if (action == "pre_execute") {
                    breakpoint_mgr_.breakOnPreExecute();
                } else if (action == "post_execute") {
                    breakpoint_mgr_.breakOnPostExecute();
                } else if (action == "pre_exception") {
                    breakpoint_mgr_.breakOnPreException();
                } else {
                    sendResponse_("ERROR: invalid action '" + args[1] + "'");
                }

                sendResponse_("OK");
            }

            // Invalid!
            else {
                sendResponse_("ERROR: invoke command as 'break [info|delete [n]|exception|<inst>|pc <pc>|disable <n>|enable <n]'");
            }
        }

        else if (args.size() == 3) {
            // "break pc >= 12345"
            if (args[0] == "pc") {
                static const std::unordered_set<std::string> comparators = {"==", "!=", "<", ">", "<=", ">="};
                if (comparators.find(args[1]) == comparators.end()) {
                    sendResponse_("ERROR: invalid comparison operator '" + args[1] + "'");
                    return;
                }
                breakpoint_mgr_.breakPC(std::stoull(args[2]), args[1]);
                sendResponse_("OK");
            }

            // Invalid!
            else {
                sendResponse_("ERROR: invoke command as 'break [info|delete [n]|exception|<inst>|pc [==,!=,<,>,<=,>=] <pc]'");
            }
        }

        // Invalid!
        else {
            sendResponse_("ERROR: invoke command as 'break [info|delete [n]|exception|<inst>|pc [==,!=,<,>,<=,>=] <pc>]'");
        }
    }

    void handleFinishRequest_(AtlasState*, const std::vector<std::string>&)
    {
        breakpoint_mgr_.disableAllBreakpoints();
    }

    void handleExitRequest_(AtlasState* state, const std::vector<std::string>&)
    {
        auto inst_action_group = state->getCurrentInst()->getActionGroup();
        auto stop_action_group = state->getStopSimActionGroup();
        inst_action_group->setNextActionGroup(stop_action_group);
        breakpoint_mgr_.disableAllBreakpoints();
    }

    std::string host_;
    int port_;
    int server_fd_;
    int new_socket_;
    std::unordered_set<std::string> insts_being_edited_;
    BreakpointManager breakpoint_mgr_;
};

SimController::SimController()
    : endpoint_(std::make_shared<SimEndpoint>())
{
    endpoint_->openEndpoint();
}

void SimController::postInit(AtlasState* state)
{
    endpoint_->postInit(state);
}

ActionGroup* SimController::preExecute(AtlasState* state)
{
    return endpoint_->preExecute(state);
}

ActionGroup* SimController::postExecute(AtlasState* state)
{
    return endpoint_->postExecute(state);
}

ActionGroup* SimController::preException(AtlasState* state)
{
    return endpoint_->preException(state);
}

void SimController::onSimulationFinished(AtlasState* state)
{
    endpoint_->onSimulationFinished(state);
}

} // namespace atlas
