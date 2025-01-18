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

// These methods are in SimControllerJSON.cpp
extern std::string getSimStatusJson(AtlasState::SimState* sim_state);
extern std::string getInstJson(AtlasInst* insn);
extern std::string getRegisterSetJson(RegisterSet* rset);
extern std::string getRegisterJson(sparta::Register* reg);
extern std::string getBreakpointsJson();
// -----------------------------------------

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
            close(server_fd_);
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(server_fd_, 1) < 0) {
            perror("Listen failed");
            close(server_fd_);
            exit(EXIT_FAILURE);
        }

        std::cout << "Waiting for connection...\n";

        // Accept an incoming connection
        new_socket_ = accept(server_fd_, nullptr, nullptr);
        if (new_socket_ < 0) {
            perror("Accept failed");
            close(server_fd_);
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
            // { "action": "pre_execute" }
            const auto json = "{\"action\": \"pre_execute\"}";
            sendJson_(json);
            return enterLoop_(state);
        }
        return nullptr;
    }

    ActionGroup* postExecute(AtlasState* state)
    {
        if (breakpoint_mgr_.shouldBreakOnPostExecute(state)) {
            // { "action": "post_execute" }
            const auto json = "{\"action\": \"post_execute\"}";
            sendJson_(json);
            return enterLoop_(state);
        }
        return nullptr;
    }

    ActionGroup* preException(AtlasState* state)
    {
        if (breakpoint_mgr_.shouldBreakOnPreException(state)) {
            // { "action": "pre_exception" }
            const auto json = "{\"action\": \"pre_exception\"}";
            sendJson_(json);
            return enterLoop_(state);
        }
        return nullptr;
    }

    void onSimulationFinished(AtlasState* state)
    {
        // { "action": "sim_finished" }
        const auto json = "{\"action\": \"sim_finished\"}";
        sendJson_(json);
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

    std::string receiveRequest_()
    {
        std::string response;

        char buffer[4096] = {0};
        int valread = read(new_socket_, buffer, sizeof(buffer));
        while (valread > 0) {
            response += std::string(buffer, valread);
            valread = read(new_socket_, buffer, sizeof(buffer));
        }

        return response;
    }

    void sendJson_(const std::string& message)
    {
        std::cout << "<-- Sending response: " << message << std::endl;
        send(new_socket_, message.c_str(), message.length(), 0);
    }

    // Note that the IDE doesn't really do anything with these
    // general-purpose ACKs. The reason we return something is
    // for overall consistency with the other message passing
    // code in python/C++.
    void sendAck_()
    {
        sendJson_("{\"ack\": \"ok\"}");
    }

    void sendWarning_(const std::string& warn)
    {
        sendJson_("{\"warning\": \"" + warn + "\"}");
    }

    void sendError_(const std::string& err)
    {
        sendJson_("{\"error\": \"" + err + "\"}");
    }

    // For developer use only / debugging purposes.
    void printIncomingRequest_(const std::string& request)
    {
        if (!request.empty()) {
            std::cout << "--> Received request: " << request << std::endl;
        }
    }

    ActionGroup* enterLoop_(AtlasState* state)
    {
        while (true) {
            const std::string request = receiveRequest_();
            printIncomingRequest_(request);
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
                    sendError_("'" + request + "' is not a valid command");
                    break;
            }
        }
    }

    void handleStatusRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        if (!args.empty()) {
            sendError_("'status' request expects zero arguments");
            return;
        }

        const auto sim_state = state->getSimState();
        const auto json = getSimStatusJson(sim_state);
        sendJson_(json);
    }

    void handleCurrentInstRequest_(AtlasState* state, const std::vector<std::string>& args)
    {
        if (!args.empty()) {
            sendError_("'inst' request expects zero arguments");
            return;
        }

        const AtlasInstPtr insn = state->getCurrentInst();
        const auto json = getInstJson(insn.get());
        sendJson_(json);
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
                    sendError_("invalid group number " + std::string(args[0]));
                    return;
            }

            const auto rset_json = getRegisterSetJson(rset);

            // This JSON is too large to send back over the socket without
            // requiring changes to the python code. As a workaround, we
            // will just create a random file in the tmpdir and write the
            // JSON there, and just return the path to the file.
            const auto fname = "/tmp/" + simdb::generateUUID();
            std::ofstream file(fname);
            file << rset_json;
            file.close();

            // { "regfile": "/tmp/uuid" }
            const auto response_json = "{\"regfile\": \"" + fname + "\"}";
            sendJson_(response_json);
        }

        // Invalid!
        else {
            sendError_("invoke command as 'dump <group num>'");
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
                    sendError_("invoke command as 'edit <inst> <true|false>'");
                    return;
                }
            }

            if (editable) {
                insts_being_edited_.insert(inst);
            } else {
                insts_being_edited_.erase(inst);
            }

            sendAck_();
        }

        // Invalid!
        else {
            sendError_("invoke command as 'edit <inst> [true|false]'");
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
                    sendError_("invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                const auto json = getRegisterJson(reg);
                sendJson_(json);
            } else {
                sendError_("invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendError_("invoke command as 'read <group num> <reg id>'");
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
                    sendError_("invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                reg->write(value);
                const auto json = getRegisterJson(reg);
                sendJson_(json);
            } else {
                sendError_("invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendError_("invoke command as 'write <group num> <reg id> <value>'");
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
                    sendError_("invalid group number " + std::string(args[0]));
                    return;
            }

            if (const auto reg = rset->getRegister(reg_id)) {
                reg->dmiWrite(value);
                const auto json = getRegisterJson(reg);
                sendJson_(json);
            } else {
                sendError_("invalid register id " + std::string(args[1]));
            }
        }

        // Invalid!
        else {
            sendError_("invoke command as 'write <group num> <reg id> <value>'");
        }
    }

    void handleBreakpointRequest_(AtlasState*, const std::vector<std::string>& args)
    {
        if (args.size() == 1) {
            // "break info"
            if (args[0] == "info") {
                const auto json = getBreakpointsJson();
                sendJson_(json);
            }

            // "break delete"
            else if (args[0] == "delete") {
                breakpoint_mgr_.deleteAllBreakpoints();
                sendAck_();
            }

            // "break exception"
            else if (args[0] == "exception") {
                breakpoint_mgr_.breakException();
                sendAck_();
            }

            // "break disable"
            else if (args[0] == "disable") {
                breakpoint_mgr_.disableAllBreakpoints();
                sendAck_();
            }

            // "break enable"
            else if (args[0] == "enable") {
                breakpoint_mgr_.enableAllBreakpoints();
                sendAck_();
            }

            // "break <inst>"
            else {
                breakpoint_mgr_.breakInst(args[0]);
                sendAck_();
            }
        }

        else if (args.size() == 2) {
            // "break delete [n]"
            if (args[0] == "delete") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.deleteBreakpoint(id)) {
                    sendError_("invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendAck_();
                }
            }

            // "break pc [pc]"
            else if (args[0] == "pc") {
                breakpoint_mgr_.breakPC(std::stoull(args[1]));
                sendAck_();
            }

            // "break disable <n>"
            else if (args[0] == "disable") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.disableBreakpoint(id)) {
                    sendError_("invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendAck_();
                }
            }

            // "break enable <n>"
            else if (args[0] == "enable") {
                const auto id = std::stoi(args[1]);
                if (!breakpoint_mgr_.enableBreakpoint(id)) {
                    sendError_("invalid breakpoint id " + std::string(args[1]));
                } else {
                    sendAck_();
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
                    sendError_("invalid action '" + args[1] + "'");
                }

                sendAck_();
            }

            // Invalid!
            else {
                sendError_("invoke command as 'break [info|delete [n]|exception|<inst>|pc <pc>|disable <n>|enable <n]'");
            }
        }

        else if (args.size() == 3) {
            // "break pc >= 12345"
            if (args[0] == "pc") {
                static const std::unordered_set<std::string> comparators = {"==", "!=", "<", ">", "<=", ">="};
                if (comparators.find(args[1]) == comparators.end()) {
                    sendError_("invalid comparison operator '" + args[1] + "'");
                    return;
                }
                breakpoint_mgr_.breakPC(std::stoull(args[2]), args[1]);
                sendAck_();
            }

            // Invalid!
            else {
                sendError_("invoke command as 'break [info|delete [n]|exception|<inst>|pc [==,!=,<,>,<=,>=] <pc]'");
            }
        }

        // Invalid!
        else {
            sendError_("invoke command as 'break [info|delete [n]|exception|<inst>|pc [==,!=,<,>,<=,>=] <pc>]'");
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
