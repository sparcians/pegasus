
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "system/SystemCallEmulator.hpp"
#include "sim/PegasusSim.hpp"
#include "sparta/utils/LogUtils.hpp"

#include <unistd.h>      // for write, etc
#include <fcntl.h>       // for openat, open
#include <sys/uio.h>     // for writev
#include <sys/utsname.h> // for uname
#include <string.h>
#include <sys/time.h> // get time of day
#include <sys/mman.h> // mmap
#include <sys/types.h>
#include <sys/stat.h> //fstat, etc
#include <sys/syscall.h>

#define SYSCALL_LOG(x)                                                                             \
    if (SPARTA_EXPECT_FALSE(syscall_log_))                                                         \
    {                                                                                              \
        syscall_log_ << x;                                                                         \
    }

namespace pegasus
{
    class SysCallHandlers
    {
      public:
        SysCallHandlers(SystemCallEmulator* emulator, sparta::log::MessageSource & sys_log) :
            emulator_(emulator),
            memory_map_manager_(emulator_->getMemMapParams()),
            syscall_log_(sys_log)
        {
            // Create function pointer
            auto cfp = [this]<typename FuncT>(FuncT && funct)
            { return std::bind(funct, this, std::placeholders::_1, std::placeholders::_2); };

            // RISCV64/32 SystemCall Numbers
            // https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html
            supported_sys_calls_.insert(
                {{17, {"getcwd", cfp(&SysCallHandlers::getcwd_)}},
                 {23, {"dup", cfp(&SysCallHandlers::dup_)}},
                 {25, {"stime", cfp(&SysCallHandlers::stime_)}},
                 {29, {"ioctl", cfp(&SysCallHandlers::ioctl_)}},
                 {35, {"unlinkat", cfp(&SysCallHandlers::unlinkat_)}},
                 {48, {"faccessat", cfp(&SysCallHandlers::faccessat_)}},
                 {56, {"openat", cfp(&SysCallHandlers::openat_)}},
                 {57, {"close", cfp(&SysCallHandlers::close_)}},
                 {62, {"lseek", cfp(&SysCallHandlers::lseek_)}},
                 {63, {"read", cfp(&SysCallHandlers::read_)}},
                 {64, {"write", cfp(&SysCallHandlers::write_)}},
                 {66, {"writev", cfp(&SysCallHandlers::writev_)}},
                 {67, {"pread", cfp(&SysCallHandlers::pread_)}},
                 {68, {"pwrite", cfp(&SysCallHandlers::pwrite_)}},
                 {78, {"readlinkat", cfp(&SysCallHandlers::readlinkat_)}},
                 {79, {"fstatat", cfp(&SysCallHandlers::fstatat_)}},
                 {80, {"fstat", cfp(&SysCallHandlers::fstat_)}},
                 {93, {"exit", cfp(&SysCallHandlers::exit_)}},
                 {94, {"exit_group", cfp(&SysCallHandlers::exit_group_)}},
                 {96, {"set_tid_address", cfp(&SysCallHandlers::set_tid_address_)}},
                 {98, {"futex", cfp(&SysCallHandlers::futex_)}},
                 {99, {"set_robust_list", cfp(&SysCallHandlers::set_robust_list_)}},
                 {113, {"clock_gettime", cfp(&SysCallHandlers::clock_gettime_)}},
                 {131, {"tgkill", cfp(&SysCallHandlers::tgkill_)}},
                 {134, {"rt_sigaction", cfp(&SysCallHandlers::rt_sigaction_)}},
                 {135, {"rt_sigprocmask", cfp(&SysCallHandlers::rt_sigprocmask_)}},
                 {160, {"uname", cfp(&SysCallHandlers::uname_)}},
                 {172, {"getpid", cfp(&SysCallHandlers::getpid_)}},
                 {174, {"getuid", cfp(&SysCallHandlers::getuid_)}},
                 {175, {"geteuid", cfp(&SysCallHandlers::geteuid_)}},
                 {176, {"getgid", cfp(&SysCallHandlers::getgid_)}},
                 {177, {"gettid", cfp(&SysCallHandlers::gettid_)}},
                 {178, {"getegid", cfp(&SysCallHandlers::getegid_)}},
                 {214, {"brk", cfp(&SysCallHandlers::brk_)}},
                 {215, {"munmap", cfp(&SysCallHandlers::munmap_)}},
                 {222, {"mmap", cfp(&SysCallHandlers::mmap_)}},
                 {226, {"mprotect", cfp(&SysCallHandlers::mprotect_)}},
                 {258, {"hwprobe", cfp(&SysCallHandlers::hwprobe_)}},
                 {261, {"prlimit", cfp(&SysCallHandlers::prlimit_)}},
                 {278, {"getrandom", cfp(&SysCallHandlers::getrandom_)}},
                 {291, {"statx", cfp(&SysCallHandlers::statx_)}},
                 {403,
                  {"clock_gettime",
                   cfp(&SysCallHandlers::clock_gettime_)}}, // sc_call_id = 403 is for
                                                            // "clock_gettime64".
                 {435, {"clone", cfp(&SysCallHandlers::clone_)}},
                 {1024, {"open", cfp(&SysCallHandlers::open_)}},
                 {1039, {"lstat", cfp(&SysCallHandlers::lstat_)}},
                 {2011, {"getmainvars", cfp(&SysCallHandlers::getmainvars_)}}});
        }

        // Pulled from https://www.man7.org/linux/man-pages/man2/statx.2.html
        struct RV_statx_timestamp
        {
            int64_t tv_sec;   // Seconds since the Epoch
            uint32_t tv_nsec; // Nanoseconds
        };

        struct RV_statx
        {
            uint32_t stx_mask;
            uint32_t stx_blksize;
            uint64_t stx_attributes;
            uint32_t stx_nlink;
            uint32_t stx_uid;
            uint32_t stx_gid;
            uint16_t stx_mode;
            uint64_t stx_ino;
            uint64_t stx_size;
            uint64_t stx_blocks;
            uint64_t stx_attributes_mask;

            struct RV_statx_timestamp stx_atime;
            struct RV_statx_timestamp stx_btime;
            struct RV_statx_timestamp stx_ctime;
            struct RV_statx_timestamp stx_mtime;

            uint32_t stx_rdev_major;
            uint32_t stx_rdev_minor;
            uint32_t stx_dev_major;
            uint32_t stx_dev_minor;

            uint64_t stx_mnt_id;

            uint32_t stx_dio_mem_align;
            uint32_t stx_dio_offset_align;

            uint64_t stx_subvol;

            uint32_t stx_atomic_write_unit_min;
            uint32_t stx_atomic_write_unit_max;
            uint32_t stx_atomic_write_segments_max;
            uint32_t stx_dio_read_offset_align;
            uint32_t stx_atomic_write_unit_max_opt;
        };

        int64_t emulateSystemCall(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF* memory)
        {
            int64_t ret_val = -1;
            const auto sc_call_id = call_stack[0];
            try
            {
                const auto & syscall = supported_sys_calls_.at(sc_call_id);
                ret_val = syscall.handler(call_stack, memory);
            }
            catch (const std::out_of_range &)
            {
                sparta_assert(false, "System call #" << sc_call_id << " is not known");
            }
            return ret_val;
        }

        void setWorkload(const std::string & workload) { workload_ = workload; }

        void setBreakAddress(Addr addr) { brk_address_ = addr; }

        Addr getBreakAddress() const { return brk_address_; }

      private:
        // Helpers
        std::string readString_(sparta::memory::BlockingMemoryIF* mem, uint64_t string_addr,
                                uint64_t string_len = 0) const
        {
            std::string ret_string;

            if (string_len == 0)
            {
                // Read the string address up to a reasonable limit.
                const uint32_t reasonable_string_limit = 1024;

                // String length uknown.  Read until we come across
                // null
                uint32_t byte = 0;
                while (byte < reasonable_string_limit)
                {
                    uint8_t one_char;
                    mem->peek(string_addr, 1, &one_char);
                    if (one_char == 0)
                    {
                        break;
                    }
                    ret_string += one_char;
                    ++byte;
                    ++string_addr;
                }
                sparta_assert(byte != reasonable_string_limit,
                              "Attempting to get a string from memory that's larger than "
                                  << reasonable_string_limit << " Got so far: " << ret_string);
            }
            else
            {
                // Systemcall length does not count the "null". But we need to accout for that here.
                unsigned char* string_in_memory =
                    static_cast<unsigned char*>(alloca(string_len + 1));
                string_in_memory[string_len] = '\0';
                if (SPARTA_EXPECT_FALSE(mem->doesAccessSpan(string_addr, string_len)))
                {
                    const auto mem_block_size = mem->getBlockSize();

                    // Need to break up the read into parts.
                    const uint32_t first_half_length =
                        mem_block_size - (string_addr & (mem_block_size - 1));
                    mem->peek(string_addr, first_half_length, string_in_memory);
                    mem->peek(string_addr + first_half_length, string_len - first_half_length,
                              string_in_memory + first_half_length);
                }
                else
                {
                    mem->peek(string_addr, string_len, string_in_memory);
                }
                ret_string = reinterpret_cast<char*>(string_in_memory);
            }
            return ret_string;
        }

        // Convert Linux ret to errno value for internal system calls
        int sysretErrno_(int ret) const { return (ret == -1) ? -errno : ret; }

        // The system calls
        int64_t getcwd_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t dup_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t ioctl_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t unlinkat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t faccessat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
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
        int64_t exit_group_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t set_tid_address_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t futex_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t set_robust_list_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t clock_gettime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t tgkill_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t rt_sigaction_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t rt_sigprocmask_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getpid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t uname_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t geteuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getgid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t gettid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getegid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t brk_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t mmap_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t munmap_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t mprotect_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t hwprobe_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t prlimit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getrandom_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t statx_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t clone_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t open_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getmainvars_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);

        // The parent emulator
        SystemCallEmulator* emulator_ = nullptr;

        // Callbacks
        using HandlerFunc =
            std::function<int64_t(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)>;

        struct SystemCall
        {
            const std::string name;
            const HandlerFunc handler;
        };

        std::unordered_map<uint64_t, SystemCall> supported_sys_calls_;

        // Memory management for things like mmap, etc
        class MemoryManager
        {
          public:
            MemoryManager(const std::vector<uint64_t> & mmap_params) :
                next_block_addr_(mmap_params.at(0)),
                base_addr_(mmap_params.at(0)),
                total_size_(mmap_params.at(1)),
                page_size_(mmap_params.at(2))
            {
            }

            // Return the guest address
            uint64_t allocate(uint64_t host_addr, uint64_t total_size)
            {
                // Aligned the requesting total_size with page_size_
                total_size = ((total_size - 1) & ~(page_size_ - 1)) + page_size_;
                sparta_assert(((next_block_addr_ + total_size) < (base_addr_ + total_size_)),
                              "Requested memory block larger than what this MemoryManager expects:"
                              "\tAsked total_size: "
                                  << total_size << " expected: " << total_size_);
                sparta_assert(guest_to_host_mapping_.find(next_block_addr_)
                                  == guest_to_host_mapping_.end(),
                              "Double allocations for address: " << HEX16(next_block_addr_));
                guest_to_host_mapping_.insert(std::make_pair(next_block_addr_, host_addr));
                const auto ret_addr = next_block_addr_;
                next_block_addr_ += total_size; // Move to the next block
                return ret_addr;
            }

            // Return the host address
            uint64_t deallocate(uint64_t guest_addr, uint64_t)
            {
                if (guest_to_host_mapping_.find(guest_addr) != guest_to_host_mapping_.end())
                {
                    const auto ret_addr = guest_to_host_mapping_[guest_addr];
                    guest_to_host_mapping_.erase(guest_addr);
                    return ret_addr;
                }
                // Nothing to unmap, return success
                return 0;
            }

          private:
            uint64_t next_block_addr_ = 0;
            const uint64_t base_addr_;
            const uint64_t total_size_;
            const uint16_t page_size_;

            // The mapping from guest address to host address
            std::unordered_map<uint64_t, uint64_t> guest_to_host_mapping_;
        } memory_map_manager_;

        // Logging
        sparta::log::MessageSource & syscall_log_;

        // The workload
        std::string workload_;

        // For a program, if the `brk` system call is made, the
        // program is asking to extend the data segment
        Addr brk_address_ = 0;
    };

    const std::string getWorkloadParam(sparta::TreeNode* rtn)
    {
        auto extension = sparta::notNull(rtn->getExtension("sim"));
        const auto & workloads_and_args =
            extension->getParameters()
                ->getParameter("workloads")
                ->getValueAs<PegasusSimParameters::WorkloadsAndArgs>();
        if (workloads_and_args.empty())
        {
            return "";
        }
        return workloads_and_args.at(0).at(0);
    }

    SystemCallEmulator::SystemCallEmulator(
        sparta::TreeNode* my_node, const SystemCallEmulator::SystemCallEmulatorParameters* p) :
        sparta::Unit(my_node),
        syscall_emulation_enabled_(
            PegasusSimParameters::getParameter<bool>(my_node, "enable_syscall_emulation")),
        memory_map_params_(p->mem_map_params),
        syscall_log_(my_node, "syscall", "System Call Logger"),
        workload_(getWorkloadParam(my_node->getRoot()))
    {
        sim_ = dynamic_cast<PegasusSim*>(my_node->getRoot()->getSimulation());

        if (p->write_output == "-")
        {
            fd_for_write_ = SystemCallEmulator::DEFAULT_WRITE_FD;
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

        callbacks_ = std::make_unique<SysCallHandlers>(this, syscall_log_);
        if (workload_.empty() == false)
        {
            callbacks_->setWorkload(workload_);
        }
    }

    SystemCallEmulator::~SystemCallEmulator()
    {
        if (nullptr != file_for_write_)
        {
            ::fclose(file_for_write_);
        }
    }

    void SystemCallEmulator::onBindTreeLate_()
    {
        if (syscall_emulation_enabled_)
        {
            const auto & symbols = sim_->getPegasusSystem()->getSymbols();
            for (auto symbol : symbols)
            {
                if (symbol.second == "_end")
                {
                    callbacks_->setBreakAddress(symbol.first);
                    break;
                }
            }
            sparta_assert(callbacks_->getBreakAddress() != 0,
                          "Could not find _end symbol in workload for system call emulation");
        }
    }

    int64_t SystemCallEmulator::emulateSystemCall(const SystemCallStack & call_stack,
                                                  sparta::memory::BlockingMemoryIF* memory)
    {
        return callbacks_->emulateSystemCall(call_stack, memory);
    }

    int SystemCallEmulator::getFDOverrideForWrite(int caller_fd)
    {
        int fd = caller_fd;
        // User may want to redirect all writes somewhere else...
        if (SPARTA_EXPECT_FALSE(fd_for_write_ != SystemCallEmulator::DEFAULT_WRITE_FD))
        {
            fd = fd_for_write_;
        }
        return fd;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // The system calls
    int64_t SysCallHandlers::getcwd_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        const auto pbuf = call_stack[1];
        const auto size = call_stack[2];
        int64_t ret = pbuf;
        std::vector<uint8_t> final_buf(size);
        char* local_ret = ::getcwd((char*)final_buf.data(), size);
        if (local_ret == (char*)final_buf.data())
        {
            size_t slen = strnlen((char*)final_buf.data(), size);
            mem->poke(pbuf, slen, final_buf.data());
            ret = slen;
        }
        else
        {
            ret = 0;
        }
        SYSCALL_LOG("getcwd(" << HEX16(pbuf) << ", " << HEX16(size) << ") -> " << HEX16(ret));

        return ret;
    }

    int64_t SysCallHandlers::dup_(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF*)
    {
        return ::dup(call_stack[1]);
    }

    int64_t SysCallHandlers::getuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        return ::getuid();
    }

    int64_t SysCallHandlers::geteuid_(const SystemCallStack & call_stack,
                                      sparta::memory::BlockingMemoryIF* memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::getgid_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::gettid_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::getegid_(const SystemCallStack & call_stack,
                                      sparta::memory::BlockingMemoryIF* memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::brk_(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF*)
    {
        if (call_stack[1] != 0)
        {
            // Set new address
            brk_address_ = call_stack[1];
        }
        // else request for address

        const int64_t ret = brk_address_;

        SYSCALL_LOG(__func__ << "(" << HEX16(ret) << ") -> " << ret);
        return ret;
    }

    int64_t SysCallHandlers::mmap_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF*)
    {
        const auto addr = call_stack[1];
        const auto size = call_stack[2];
        const auto prot = call_stack[3];
        const auto flags = call_stack[4];
        const auto fd = call_stack[5];
        const auto offset = call_stack[6];

        if (addr != 0)
        {
            std::cerr << "ATHENA: WARNING: mmap system call with non-zero starting addr. "
                      << "Not well supported" << std::endl;
        }

        uint64_t guest_addr = 0;

        if ((flags & MAP_ANONYMOUS) == 0)
        {
            sparta_assert(fd != std::numeric_limits<uint64_t>::max(),
                          "Memory map to file -- not supported");
        }

        guest_addr = memory_map_manager_.allocate(0ull, size);

        SYSCALL_LOG("mmap(" << HEX16(addr) << ", " << HEX16(size) << ", " << HEX16(prot) << ", "
                            << HEX16(flags) << ", " << HEX16(fd) << ", " << HEX16(offset) << ", "
                            << ") -> " << guest_addr);

        return guest_addr;
    }

    int64_t SysCallHandlers::munmap_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF*)
    {
        const auto guest_addr = call_stack[1];
        const auto size = call_stack[2];
        const auto host_addr = memory_map_manager_.deallocate(guest_addr, size);

        SYSCALL_LOG("munmap(" << HEX16(guest_addr) << ", " << HEX16(size) << ") -> " << host_addr);

        return 0;
    }

    int64_t SysCallHandlers::mprotect_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::hwprobe_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::prlimit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::getrandom_(const SystemCallStack & call_stack,
                                        sparta::memory::BlockingMemoryIF* mem)
    {
        auto buffer = call_stack[1];
        auto buflen = call_stack[2];

        // Use cycles to provide a deterministic answer
        do
        {
            uint64_t cycle = emulator_->getClock()->currentCycle();
            cycle = cycle * 6364136223846793005 + 1442695040888963407; // knuth random seed

            mem->poke(buffer, buflen, (uint8_t*)&cycle);

            if (buflen <= sizeof(cycle))
                break;

            buffer += sizeof(cycle);
            buflen -= sizeof(cycle);
        } while (true);

        return buflen;
    }

    int64_t SysCallHandlers::stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::ioctl_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::unlinkat_(const SystemCallStack & call_stack,
                                       sparta::memory::BlockingMemoryIF* mem)
    {

        const auto dirfd = call_stack[1];
        const auto pathname_addr = call_stack[2];
        const auto flags = call_stack[3];
        const auto pathname_str = readString_(mem, pathname_addr, 0);

        auto ret = ::unlinkat(dirfd, pathname_str.c_str(), flags);

        SYSCALL_LOG(__func__ << "(" << dirfd << "," << HEX16(pathname_addr) << "['" << pathname_str
                             << "']"
                             << "-> " << ret);
        return ret;
    }

    int64_t SysCallHandlers::faccessat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::openat_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        const auto dirfd = call_stack[1];
        const auto pathname_addr = call_stack[2];
        const auto flags = call_stack[3];
        const auto mode = call_stack[4];

        const std::string pathname = readString_(mem, pathname_addr);

        auto ret = ::openat(dirfd, pathname.c_str(), flags, mode);

        return ret;
    }

    int64_t SysCallHandlers::close_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF*)
    {
        const auto fd = call_stack[1];
        auto ret = sysretErrno_(::close(fd));
        return ret;
    }

    int64_t SysCallHandlers::lseek_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF*)
    {
        const auto fd = call_stack[1];
        const auto offset = call_stack[2];
        const auto whence = call_stack[3];

        int64_t ret = sysretErrno_(::lseek(fd, offset, whence));

        SYSCALL_LOG("lseek(" << HEX16(fd) << ", " << HEX16(offset) << ", " << HEX16(whence) << ", "
                             << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::read_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF* mem)
    {
        const auto fd = call_stack[1];
        const auto buf = call_stack[2];
        const auto count = call_stack[3];

        std::vector<uint8_t> final_buf(count);
        int64_t ret = ::read(fd, (char*)final_buf.data(), count);
        if (ret > 0)
        {
            mem->poke(buf, count, final_buf.data());
        }
        ret = sysretErrno_(ret);

        SYSCALL_LOG("read(" << HEX16(fd) << ", " << HEX16(buf) << ", " << HEX16(count) << ", "
                            << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::write_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* mem)
    {
        int fd = emulator_->getFDOverrideForWrite(call_stack[1]);
        const auto string_addr = call_stack[2];
        const auto string_len = call_stack[3];
        std::string str;
        // Check that the file descriptor is valid
        int64_t ret = fcntl(fd, F_GETFD);
        if (ret != -1)
        {
            str = readString_(mem, string_addr, string_len);

            // send the string to the file descriptor
            ret = sysretErrno_(::write(fd, str.c_str(), string_len));
        }
        SYSCALL_LOG("write(" << fd << ", " << HEX16(string_addr) << "['" << str << "'], "
                             << string_len << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::writev_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        auto fd = emulator_->getFDOverrideForWrite(call_stack[1]);
        const auto iov_addr = call_stack[2];
        const auto iov_cnt = call_stack[3];

        // Grab the iovec structure from mem
        iovec* iov = (iovec*)alloca(sizeof(iovec) * iov_cnt);
        mem->peek(iov_addr, sizeof(iovec) * iov_cnt, (uint8_t*)(iov));

        int64_t ret = 0;
        for (uint32_t i = 0; i < iov_cnt; ++i)
        {
            const uint64_t str_addr = (uint64_t)iov[i].iov_base;
            const auto str_len = iov[i].iov_len;
            std::vector<uint8_t> char_str(str_len);
            if (SPARTA_EXPECT_FALSE(mem->doesAccessSpan(str_addr, str_len)))
            {
                const auto mem_block_size = mem->getBlockSize();

                // Need to break up the read into parts.
                // Math: 0x400 - (0x3fe & 0x3ff) = 2 -- will fetch 2 bytes
                const uint32_t len_first_half = mem_block_size - (str_addr & (mem_block_size - 1));

                mem->peek(str_addr, len_first_half, &char_str[0]);

                mem->peek(str_addr + len_first_half, str_len - len_first_half,
                          &char_str[0] + len_first_half);
            }
            else
            {
                mem->peek(str_addr, str_len, &char_str[0]);
            }

            ret += ::write(fd, &char_str[0], str_len);
        }
        SYSCALL_LOG("writev(" << fd << ", " << HEX16(iov_addr) << ", " << iov_cnt << ") -> "
                              << ret);
        return ret;
    }

    int64_t SysCallHandlers::pread_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::pwrite_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::readlinkat_(const SystemCallStack & call_stack,
                                         sparta::memory::BlockingMemoryIF* mem)
    {
        int64_t ret = -1;
        const auto dirfd = call_stack[1];
        const auto pathname = call_stack[2];
        const auto buf = call_stack[3];
        const auto bufsize = call_stack[4];

        std::string pathname_in_memory;
        uint8_t one_char = 0;
        uint32_t next_byte = 0;
        do
        {
            mem->peek(pathname + next_byte, 1, &one_char);
            if (one_char != 0)
            {
                pathname_in_memory += one_char;
            }
            else
            {
                // end of string
                break;
            }
            ++next_byte;
        } while (true);

        std::vector<uint8_t> final_resolved_path(bufsize, '\0');
        auto final_resolved_path_ptr = (char*)final_resolved_path.data();

        ret = 0;
        if ("/proc/self/exe" == pathname_in_memory)
        {
            auto was_resolved = ::realpath(workload_.c_str(), final_resolved_path_ptr);
            if (was_resolved != final_resolved_path_ptr)
            {
                ret = -errno;
            }
            else
            {
                ret = strlen(final_resolved_path_ptr);
            }
        }
        else
        {
            ret = sysretErrno_(
                ::readlinkat(dirfd, pathname_in_memory.c_str(), final_resolved_path_ptr, bufsize));
        }

        if (ret > 0)
        {
            mem->poke(buf, bufsize, (uint8_t*)final_resolved_path_ptr);
        }

        SYSCALL_LOG("readlinkat(" << HEX16(dirfd) << ", " << HEX16(pathname) << "(\""
                                  << pathname_in_memory << "\"), " << HEX16(buf) << "(\""
                                  << final_resolved_path_ptr << "\"), " << bufsize << ") -> "
                                  << ret);
        return ret;
    }

    // Pulled from /usr/riscv64-linux-gnu/include/sys/stat.h and
    // frontend.h from pk
    struct RV_stat
    {
        uint64_t st_dev;
        uint64_t st_ino;
        uint32_t st_mode;
        uint32_t st_nlink;
        uint32_t st_uid;
        uint32_t st_gid;
        uint64_t st_rdev;
        uint64_t __pad1;
        uint64_t st_size;
        uint32_t st_blksize;
        uint32_t __pad2;
        uint64_t st_blocks;
        struct timespec st_atim;
        struct timespec st_mtim;
        struct timespec st_ctim;
        int __glibc_reserved[2];

        RV_stat(const struct stat & host_stat) :
            st_dev(host_stat.st_dev),
            st_ino(host_stat.st_ino),
            st_mode(host_stat.st_mode),
            st_nlink(host_stat.st_nlink),
            st_uid(host_stat.st_uid),
            st_gid(host_stat.st_gid),
            st_rdev(host_stat.st_rdev),
            st_size(host_stat.st_size),
            st_blksize(host_stat.st_blksize),
            st_blocks(host_stat.st_blocks),
#ifdef __APPLE__
            st_atim(0),
            st_mtim(0),
            st_ctim(0)
#else
            st_atim(host_stat.st_atim),
            st_mtim(host_stat.st_mtim),
            st_ctim(host_stat.st_ctim)
#endif
        {
        }
    };

    int64_t SysCallHandlers::fstatat_(const SystemCallStack & call_stack,
                                      sparta::memory::BlockingMemoryIF* memory)
    {
        int64_t ret = 0;

        const auto dirfd = call_stack[1];
        const auto pathname = call_stack[2];
        const auto statbuf = call_stack[3];
        const auto flags = call_stack[4];
        const std::string pathname_str = readString_(memory, pathname, 0);

        // Use the host's stat
        struct stat host_stat;

        ret = sysretErrno_(::fstatat(dirfd, pathname_str.c_str(), &host_stat, flags));

        if (ret != -1)
        {
            // Now create a RV stat and populate
            RV_stat rv_stat(host_stat);

            memory->poke(statbuf, sizeof(struct RV_stat), reinterpret_cast<uint8_t*>(&rv_stat));
        }

        SYSCALL_LOG(__func__ << "(" << HEX16(dirfd) << "["
                             << (int(dirfd) == AT_FDCWD ? "AT_FDCWD" : "stdio or fd") << "], "
                             << HEX16(pathname) << "(\"" << pathname_str << "\"), "
                             << HEX16(statbuf) << ", " << HEX16(flags) << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::fstat_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* memory)
    {
        int64_t ret = 0;

        const auto fd = call_stack[1];
        const auto statbuf = call_stack[2];

        // Use the host's stat
        struct stat host_stat;
        ret = sysretErrno_(::fstat(fd, &host_stat));

        if (ret != -1)
        {
            // Now create a RV stat and populate
            RV_stat rv_stat(host_stat);
            memory->poke(statbuf, sizeof(struct RV_stat), reinterpret_cast<uint8_t*>(&rv_stat));
        }

        SYSCALL_LOG(__func__ << "(" << HEX16(fd) << ", " << HEX16(statbuf) << ") -> " << ret);
        return ret;
    }

    int64_t SysCallHandlers::set_tid_address_(const SystemCallStack &,
                                              sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::futex_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::set_robust_list_(const SystemCallStack &,
                                              sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::clock_gettime_(const SystemCallStack & call_stack,
                                            sparta::memory::BlockingMemoryIF* memory)
    {
        // To be used once we're using the sparta scheduler
        // auto * clk = emulator_->getClock();
        // uint32_t clk_freq = (uint32_t)clk->getFrequencyMhz();
        // uint32_t cyc = (uint32_t)clk->currentCycle();

        // // timespec is 2 uint32_t structures
        // struct timespec tm = {
        //     cyc / freq,
        //     uint32_t((cyc % freq) / (clk->getFrequencyMhz() / 1000000000))
        // };

        struct timespec tm;
        ::clock_gettime((clockid_t)call_stack[0], &tm);

        SYSCALL_LOG(__func__ << "(" << HEX16(call_stack[0]) << ", " << HEX16(call_stack[1]) << ", "
                             << ") -> 0");

        memory->poke(call_stack[1], sizeof(tm), reinterpret_cast<uint8_t*>(&tm));

        return 0;
    }

    int64_t SysCallHandlers::tgkill_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        return exit_(call_stack, mem);
    }

    int64_t SysCallHandlers::rt_sigaction_(const SystemCallStack &,
                                           sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::rt_sigprocmask_(const SystemCallStack &,
                                             sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::getpid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        auto ret = ::getpid();
        return ret;
    }

    int64_t SysCallHandlers::uname_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* memory)
    {
        const auto uname_data_ptr = call_stack[1];
        const auto UTS_CHAR_LENGTH = 64 + 1; // standard

        struct uname_info
        {
            const char sysname[UTS_CHAR_LENGTH] = "Pegasus Core Emulator";
            const char nodename[UTS_CHAR_LENGTH] = "";
            const char release[UTS_CHAR_LENGTH] = "4.15.0";
            const char version[UTS_CHAR_LENGTH] = "";
            const char machine[UTS_CHAR_LENGTH] = "";
            const char domainname[UTS_CHAR_LENGTH] = ""; // Domainname (if exists)
        } pegasus_uname_info;

        memory->poke(uname_data_ptr, sizeof(uname_info),
                     reinterpret_cast<uint8_t*>(&pegasus_uname_info));
        SYSCALL_LOG("uname(" << HEX16(uname_data_ptr) << ") -> 0");
        return 0;
    }

    int64_t SysCallHandlers::exit_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF*)
    {
        const int64_t exit_code = call_stack[1];
        SYSCALL_LOG("exit(" << exit_code << ");");
        emulator_->getPegasusSim()->endSimulation(exit_code);
        return exit_code;
    }

    int64_t SysCallHandlers::exit_group_(const SystemCallStack & call_stack,
                                         sparta::memory::BlockingMemoryIF* memory)
    {
        const int64_t exit_code = call_stack[1];
        SYSCALL_LOG("exit_group(" << exit_code << ");");
        return exit_(call_stack, memory);
    }

    int64_t SysCallHandlers::statx_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* memory)
    {
        int64_t ret = 0;
        const auto dirfd = call_stack[1];
        const auto pathname = call_stack[2];
        const auto flags = call_stack[3];
        const auto mask = call_stack[4];
        const auto statxbuf = call_stack[5];
        const std::string pathname_str = readString_(memory, pathname, 0);

        struct RV_statx rv_host_stat;

        // To hold statx struct
        // statx host_stat;
        // FIXME: statx struct not defined in sys/stat.h, so just allocate enough space
        // for the syscall to write the unknown struct to, 300B should be enough!
        std::vector<uint8_t> host_stat;
        host_stat.reserve(300);
        // ret = sysretErrno_(::statx(dirfd, pathname_str.c_str(), flags, mask, &host_stat));

#ifdef __APPLE__
        // syscall is not supported on MacOS
        ret = -1;
#else

#ifndef SYS_statx
        constexpr int SYS_statx = 332; // x86_64-specific syscall number for statx;
                                       // used for manually invoking via syscall()
#endif
        ret = sysretErrno_(
            syscall(SYS_statx, dirfd, pathname_str.c_str(), flags, mask, host_stat.data()));
#endif

        if (ret != -1)
        {
            memory->poke(statxbuf, sizeof(struct RV_stat),
                         reinterpret_cast<uint8_t*>(&rv_host_stat));
        }

        SYSCALL_LOG(__func__ << "(" << HEX16(dirfd) << "["
                             << (int(dirfd) == AT_FDCWD ? "AT_FDCWD" : "stdio or fd") << "], "
                             << HEX16(pathname) << "(\"" << pathname_str << "\"), "
                             << HEX16(statxbuf) << ", " << HEX16(flags) << "," << HEX16(mask)
                             << ") -> " << ret);
        return ret;
    }

    struct clone_args
    {
        uint64_t flags;        /* Flags bit mask */
        uint64_t pidfd;        /* Where to store PID file descriptor
                                  (int *) */
        uint64_t child_tid;    /* Where to store child TID,
                                  in child's memory (pid_t *) */
        uint64_t parent_tid;   /* Where to store child TID,
                                  in parent's memory (pid_t *) */
        uint64_t exit_signal;  /* Signal to deliver to parent on
                                  child termination */
        uint64_t stack;        /* Pointer to lowest byte of stack */
        uint64_t stack_size;   /* Size of stack */
        uint64_t tls;          /* Location of new TLS */
        uint64_t set_tid;      /* Pointer to a pid_t array
                                  (since Linux 5.5) */
        uint64_t set_tid_size; /* Number of elements in set_tid
                                  (since Linux 5.5) */
        uint64_t cgroup;       /* File descriptor for target cgroup
                                  of child (since Linux 5.7) */
    };

    int64_t SysCallHandlers::clone_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* memory)
    {
        // Get clone args struct from memory
        clone_args args;
        const uint64_t addr = call_stack[1];
        const size_t size = call_stack[2];
        sparta_assert(sizeof(clone_args) == size,
                      "Size of clone_args struct does not match size parameter!");
        const bool success = memory->tryRead(addr, size, reinterpret_cast<uint8_t*>(&args));
        sparta_assert(success);

        SYSCALL_LOG(__func__ << " clone_args: flags: 0x" << std::hex << args.flags);
        SYSCALL_LOG(__func__ << " clone_args: pidfd: 0x" << std::hex << args.pidfd);
        SYSCALL_LOG(__func__ << " clone_args: child_tid: 0x" << std::hex << args.child_tid);
        SYSCALL_LOG(__func__ << " clone_args: parent_tid: 0x" << std::hex << args.parent_tid);
        SYSCALL_LOG(__func__ << " clone_args: exit_signal: 0x" << std::hex << args.exit_signal);
        SYSCALL_LOG(__func__ << " clone_args: stack: 0x" << std::hex << args.stack);
        SYSCALL_LOG(__func__ << " clone_args: stack_size: 0x" << std::hex << args.stack_size);
        SYSCALL_LOG(__func__ << " clone_args: tls: 0x" << std::hex << args.tls);
        SYSCALL_LOG(__func__ << " clone_args: set_tid: 0x" << std::hex << args.set_tid);
        SYSCALL_LOG(__func__ << " clone_args: set_tid_size: 0x" << std::hex << args.set_tid_size);
        SYSCALL_LOG(__func__ << " clone_args: cgroup: 0x" << std::hex << args.cgroup);

        // TODO: Boot up a new thread, set the thread/hart ID, pause the current thread, etc.

        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::open_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF* mem)
    {
        const auto path_addr = call_stack[1];
        const auto flags = call_stack[2];
        const auto mode = call_stack[3];

        const std::string path = readString_(mem, path_addr);

        auto ret = ::open(path.c_str(), flags, mode);
        return ret;
    }

    int64_t SysCallHandlers::lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::getmainvars_(const SystemCallStack &,
                                          sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }
} // namespace pegasus
