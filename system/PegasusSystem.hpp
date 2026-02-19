#pragma once

#include "elfio/elfio.hpp"

#include "include/PegasusTypes.hpp"
#include "sim/PegasusSimParameters.hpp"
#include "system/SimpleUART.hpp"
#include "system/MagicMemory.hpp"
#include "system/ReservationMemory.hpp"

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/AddressTypes.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"

namespace sparta::memory
{
    class MemoryObject;
    class BlockingMemoryIF;
    class BlockingMemoryObjectIFNode;
    class SimpleMemoryMapNode;
} // namespace sparta::memory

namespace pegasus
{
    class Observer;

    class PegasusSystem : public sparta::Unit
    {
      public:
        //! brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "PegasusSystem";

        class PegasusSystemParameters : public sparta::ParameterSet
        {
          public:
            PegasusSystemParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(bool, enable_uart, false, "Enable a Uart")
        };

        // Constructor
        PegasusSystem(sparta::TreeNode* sys_node, const PegasusSystemParameters* p);

        // Get pointer to system memory
        sparta::memory::SimpleMemoryMapNode* getSystemMemory() { return current_map_.get(); }

        // TODO: it would be very different if SimpleMemoryMapNode has copy constructor..
        void switchSystemMemory(bool resv)
        {
            current_map_ = resv ? reservation_memory_map_ : memory_map_;
        }

        // Give observers their callbacks to read/write memory operations
        void registerMemoryCallbacks(Observer* observer);

        // Get starting PC from ELF
        Addr getStartingPc() const { return starting_pc_.isValid() ? starting_pc_.getValue() : 0; }

        // Tell the system we're using pass/fail criteria for stopping simulation
        void enableEOTPassFailMode();

        // Get all workloads and their program arguments
        const PegasusSimParameters::WorkloadsAndArgs & getWorkloadsAndArgs() const
        {
            return workloads_and_args_;
        }

        const std::unordered_map<Addr, std::string> & getSymbols() const { return symbols_; }

        constexpr static sparta::memory::addr_t PEGASUS_SYSTEM_BLOCK_SIZE = 0x1000; // 4K
        constexpr static sparta::memory::addr_t PEGASUS_SYSTEM_TOTAL_MEMORY =
            0x8000000000000000; // 4G
        constexpr static uint64_t PEGASUS_MEMORY_FILL = 0x0;

      private:
        // Device factories
        sparta::ResourceFactory<SimpleUART, SimpleUART::SimpleUARTParameters> uart_fact_;
        sparta::ResourceFactory<MagicMemory, MagicMemory::MagicMemoryParameters> magic_mem_fact_;

        // Tree nodes
        std::vector<std::unique_ptr<sparta::TreeNode>> tree_nodes_;

        // Devices
        SimpleUART* uart_ = nullptr;
        MagicMemory* magic_mem_ = nullptr;

        // Memory and memory maps
        std::shared_ptr<sparta::memory::SimpleMemoryMapNode> current_map_;
        std::shared_ptr<sparta::memory::SimpleMemoryMapNode> memory_map_;
        std::shared_ptr<ReservationMemory> reservation_memory_map_;
        std::vector<std::unique_ptr<sparta::memory::MemoryObject>> memory_objects_;

        struct MemorySection
        {
            std::string name = "?";
            sparta::memory::addr_t file_size = 0;
            sparta::memory::addr_t total_size_aligned = 0;
            sparta::memory::addr_t start_address = 0;
            const uint8_t* data = nullptr;

            MemorySection() = default;

            MemorySection(const std::string name, const sparta::memory::addr_t file_size,
                          const sparta::memory::addr_t total_size_aligned,
                          const sparta::memory::addr_t start_address, const uint8_t* data) :
                name(name),
                file_size(file_size),
                total_size_aligned(total_size_aligned),
                start_address(start_address),
                data(data)
            {
            }
        };

        std::vector<MemorySection> memory_sections_;
        sparta::utils::ValidValue<MemorySection> magic_memory_section_;

        void createMemoryMappings_(sparta::TreeNode* sys_node);

        // Workload and workload arguments
        const PegasusSimParameters::WorkloadsAndArgs workloads_and_args_;
        void loadWorkload_(const std::string & workload);
        void initMemoryWithElf_(const std::string & workload);
        ELFIO::elfio elf_reader_;

        // Binaries
        const PegasusSimParameters::Binaries binaries_;
        void loadBinary_(const std::string & binary, const Addr load_addr);

        sparta::utils::ValidValue<Addr> starting_pc_;
        std::unordered_map<Addr, std::string> symbols_;
        sparta::utils::ValidValue<Addr> tohost_addr_;
        sparta::utils::ValidValue<Addr> fromhost_addr_;
        sparta::utils::ValidValue<Addr> pass_addr_;
        sparta::utils::ValidValue<Addr> fail_addr_;
    };
} // namespace pegasus
