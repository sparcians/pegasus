#pragma once

#include <array>
#include <cinttypes>
#include <map>

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/memory/BlockingMemoryIF.hpp"
#include "sparta/utils/ValidValue.hpp"

namespace atlas
{
    class AtlasSim;

    /**
     * \class SystemCallEmulator
     *
     * \brief Utility class to handle system calls from ecall
     * operations or even magic memory
     *
     *
     */
    class SystemCallEmulator : public sparta::Unit
    {
      public:
        //! \brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "SystemCallEmulator";

        //! Default write FD, -1 assumes all writes go to the given
        //! system call provided by the call.
        static constexpr int DEFAULT_WRITE_FD = -1;

        //! Parameter to define behaviors of the State class
        class SystemCallEmulatorParameters : public sparta::ParameterSet
        {
          public:
            explicit SystemCallEmulatorParameters(sparta::TreeNode* node) : sparta::ParameterSet(node)
            {
            }
            PARAMETER(std::string, write_output, "-",
                      R"desc(For write system calls to stdout/stderr, where should the output go?
   '-'       Means to the intended target (1 for stdout, 2 for stderr, etc)
   '1'       Means ALL output should go to stdout
   '2'       Means ALL output should go to stderr
  <filename> Means ALL output to given filename)desc")
        };

        //! Construct!
        SystemCallEmulator(sparta::TreeNode* my_node, const SystemCallEmulatorParameters*);

        //! Destroy!
        ~SystemCallEmulator();

        //! Handle a system call
        using SystemCallStack = std::array<uint64_t, 8>;
        int64_t emulateSystemCall(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF* memory);

        //! Handle exit call
        void exitCall(uint64_t exit_code);

      private:
        sparta::log::MessageSource syscall_log_;
        FILE* file_for_write_ = nullptr;
        int fd_for_write_ = DEFAULT_WRITE_FD;
        AtlasSim* sim_ = nullptr;

        using HandlerFunc =
            std::function<int64_t (const SystemCallStack &, sparta::memory::BlockingMemoryIF*)>;
        struct SystemCall
        {
            const std::string name;
            const HandlerFunc handler;
        };

        //! Bad handler
        void badSystemCallHandler_(const SystemCallStack &,
                                   sparta::memory::BlockingMemoryIF*)
        {
            throw ("System Call is known, but not supported");
        }

        std::map<uint64_t, SystemCall> supported_sys_calls_;

        // The system calls
        int64_t getcwd_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t setuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t openat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t close_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t lseek_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t read_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t write_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t writev_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t pread_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t pwrite_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t readlinkat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t fstatat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t fstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t exit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t statx_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t open_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getmainvars_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
};
} // namespace atlas
