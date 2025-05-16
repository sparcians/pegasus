
#include <vector>
#include "system/SystemCallEmulator.hpp"
#include "sim/AtlasSim.hpp"
#include "sparta/utils/LogUtils.hpp"

#include <unistd.h>      // for write, etc
#include <fcntl.h>       // for openat, open
#include <sys/uio.h>     // for writev
#include <sys/utsname.h> // for uname
#include <string.h>
#include <sys/time.h>  // get time of day
#include <sys/mman.h>  // mmap

#define SYSCALL_LOG(x)                                                                           \
    if (SPARTA_EXPECT_FALSE(syscall_log_))                                                       \
    {                                                                                              \
        syscall_log_ << x;                                                                       \
    }

namespace atlas
{
    SystemCallEmulator::SystemCallEmulator(sparta::TreeNode* my_node,
                                     const SystemCallEmulator::SystemCallEmulatorParameters* p) :
        sparta::Unit(my_node),
        syscall_log_(my_node, "syscall", "System Call Logger")
    {
        sim_ = dynamic_cast<AtlasSim*>(my_node->getRoot()->getSimulation());

        if (p->write_output == "-")
        {
            fd_for_write_ = DEFAULT_WRITE_FD;
        }
        else if (p->write_output == "1")
        {
            fd_for_write_ = ::dup(1);
        }
        else if (p->write_output == "2")
        {
            fd_for_write_ = ::dup(2);
        }
        else
        {
            file_for_write_ = ::fopen(p->write_output.getValue().c_str(), "w");
            sparta_assert(file_for_write_ != nullptr, "Error: could not open '"
                          << p->write_output
                          << "' for capturing write system calls");
            fd_for_write_ = ::fileno(file_for_write_);
        }

        // Create function pointer
        auto cfp = [this] <typename FuncT> (FuncT && funct)
        {
            return std::bind(funct, this, std::placeholders::_1, std::placeholders::_2);
        };

        // RISCV64/32 SystemCall Numbers
        // https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html
        supported_sys_calls_.insert({{17, {"getcwd",        cfp(&SystemCallEmulator::getcwd_)}},
                                     {23, {"setuid",        cfp(&SystemCallEmulator::setuid_)}},
                                     {25, {"stime",         cfp(&SystemCallEmulator::stime_)}},
                                     {57, {"close",         cfp(&SystemCallEmulator::close_)}},
                                     {62, {"lseek",         cfp(&SystemCallEmulator::lseek_)}},
                                     {63, {"read",          cfp(&SystemCallEmulator::read_)}},
                                     {64, {"write",         cfp(&SystemCallEmulator::write_)}},
                                     {66, {"writev",        cfp(&SystemCallEmulator::writev_)}},
                                     {67, {"pread",         cfp(&SystemCallEmulator::pread_)}},
                                     {68, {"pwrite",        cfp(&SystemCallEmulator::pwrite_)}},
                                     {78, {"readlinkat",    cfp(&SystemCallEmulator::readlinkat_)}},
                                     {79, {"fstatat",       cfp(&SystemCallEmulator::fstatat_)}},
                                     {80, {"fstat",         cfp(&SystemCallEmulator::fstat_)}},
                                     {93, {"exit",          cfp(&SystemCallEmulator::exit_)}},
                                     {291, {"statx",        cfp(&SystemCallEmulator::statx_)}},
                                     {1024, {"open",        cfp(&SystemCallEmulator::open_)}},
                                     {1039, {"lstat",       cfp(&SystemCallEmulator::lstat_)}},
                                     {2011, {"getmainvars", cfp(&SystemCallEmulator::getmainvars_)}}});
    }

    SystemCallEmulator::~SystemCallEmulator()
    {
    }

    int64_t SystemCallEmulator::emulateSystemCall(const SystemCallStack & call_stack,
                                                  sparta::memory::BlockingMemoryIF* memory)
    {
        int64_t ret_val = -1;
        const auto sc_call_id = call_stack[0];
        try {
            const auto & syscall = supported_sys_calls_.at(sc_call_id);
            ret_val = syscall.handler(call_stack, memory);
            SYSCALL_LOG("SYSCALL: "<< syscall.name << "(" << sc_call_id << ") -> " << ret_val);
        }
        catch (...) {
            sparta_assert(false, "System call #" << sc_call_id << " is not known");
        }
        return ret_val;

    }

    // The system calls
    int64_t SystemCallEmulator::getcwd_(const SystemCallStack &call_stack,
                                        sparta::memory::BlockingMemoryIF * mem)
    {
        const auto pbuf = call_stack[1];
        const auto count = call_stack[2];
        std::vector<uint8_t> final_buf(count);
        int64_t ret = (int64_t)getcwd((char*)final_buf.data(), call_stack[2]);
        if (ret != 0)
        {
            size_t slen = strnlen((char*)final_buf.data(), call_stack[2]);
            mem->poke(pbuf, slen, final_buf.data());
        }
        if (ret)
        {
            ret = pbuf;
        }
        return ret;
    }

    int64_t SystemCallEmulator::setuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        return 0;
    }
    int64_t SystemCallEmulator::stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        return 0;
    }

    int64_t SystemCallEmulator::close_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = 0;
        return ret;
    }
    int64_t SystemCallEmulator::lseek_(const SystemCallStack &call_stack, sparta::memory::BlockingMemoryIF*)
    {
        const auto fd = call_stack[1];
        const auto offset = call_stack[2];
        const auto whence = call_stack[3];

        int64_t ret = ::lseek(fd, offset, whence);

        SYSCALL_LOG("lseek(" << HEX16(fd) << ", " << HEX16(offset) << ", "
                    << HEX16(whence) << ", "
                    << ") -> " << ret);

        return ret;
    }
    int64_t SystemCallEmulator::read_(const SystemCallStack &call_stack,
                                      sparta::memory::BlockingMemoryIF*mem)
    {
        const auto fd = call_stack[1];
        const auto buf = call_stack[2];
        const auto count = call_stack[3];

        std::vector<uint8_t> final_buf(count);
        int64_t ret = ::read(fd, (char*)final_buf.data(), count);

        mem->poke(buf, count, final_buf.data());

        SYSCALL_LOG("read(" << HEX16(fd) << ", " << HEX16(buf) << ", " << HEX16(count)
                    << ", "
                    << ") -> " << ret);

        return ret;
    }
    int64_t SystemCallEmulator::write_(const SystemCallStack & call_stack,
                                       sparta::memory::BlockingMemoryIF* mem)
    {
        auto fd = call_stack[1];
        const auto string_addr = call_stack[2];
        const auto string_len = call_stack[3];

        // User may want to redirect all writes somewhere else...
        if (SPARTA_EXPECT_FALSE(fd_for_write_ != DEFAULT_WRITE_FD)) {
            fd = fd_for_write_;
        }

        // Check that the file descriptor is valid
        int64_t ret = fcntl(fd, F_GETFD);
        if (ret != -1)
        {
            std::vector<uint8_t> string_to_write(string_len);
            if (SPARTA_EXPECT_FALSE(mem->doesAccessSpan(string_addr, string_len)))
            {
                const auto mem_block_size = mem->getBlockSize();

                // Need to break up the read into parts.
                const uint32_t first_half_length =
                    mem_block_size - (string_addr & (mem_block_size - 1));
                mem->peek(string_addr, first_half_length, &string_to_write[0]);
                mem->peek(string_addr + first_half_length, string_len - first_half_length,
                          &string_to_write[0] + first_half_length);
            }
            else
            {
                mem->peek(string_addr, string_len, &string_to_write[0]);
            }

            // send the string to the file descriptor
            ret = ::write(fd, &string_to_write[0], string_len);
        }
        SYSCALL_LOG("write(" << fd << ", " << HEX16(string_addr) << ", " << string_len
                    << ") -> " << ret);

        return ret;
    }
    int64_t SystemCallEmulator::writev_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::pread_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::pwrite_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::readlinkat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::fstatat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::fstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::exit_(const SystemCallStack &call_stack, sparta::memory::BlockingMemoryIF*)
    {
        const int64_t exit_code = call_stack[1];
        SYSCALL_LOG("exit(" << exit_code << ");");
        sim_->endSimulation(exit_code);
        return exit_code;
    }
    int64_t SystemCallEmulator::statx_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::open_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
    int64_t SystemCallEmulator::getmainvars_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        int64_t ret = -1;
        return ret;
    }
}
