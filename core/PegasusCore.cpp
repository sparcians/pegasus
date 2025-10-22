#include "PegasusCore.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    std::unordered_set<PrivMode> initSupportedPrivilegeModes(const std::string & priv)
    {
        std::unordered_set<PrivMode> priv_modes;

        for (const char mode : priv)
        {
            if (mode == 'm')
            {
                priv_modes.emplace(PrivMode::MACHINE);
            }
            else if (mode == 's')
            {
                priv_modes.emplace(PrivMode::SUPERVISOR);
            }
            else if (mode == 'u')
            {
                priv_modes.emplace(PrivMode::USER);
            }
            else
            {
                sparta_assert(false, "Unsupported privilege modes: "
                                         << priv
                                         << "\nSupported privilege modes are: Machine (M), "
                                            "Supervisor (S) and User (U)");
            }
        }

        return priv_modes;
    }

    uint32_t getXlenFromIsaString(const std::string & isa_string)
    {
        if (isa_string.find("32") != std::string::npos)
        {
            return 32;
        }
        else if (isa_string.find("64") != std::string::npos)
        {
            return 64;
        }
        else
        {
            sparta_assert(false, "Failed to determine XLEN from ISA string: " << isa_string);
        }
    }

    bool PegasusCore::validateISAString_(std::string & unsupportedExt)
    {
        const auto & supported_exts =
            (xlen_ == 64) ? supported_rv64_extensions_ : supported_rv32_extensions_;

        const auto hasExtension = [&](const std::string & ext) {
            return std::find(supported_exts.begin(), supported_exts.end(), ext)
                   != supported_exts.end();
        };

        for (const auto & ext : extension_manager_.getEnabledExtensions(false))
        {
            if (!hasExtension(ext.first))
            {
                unsupportedExt = ext.first;
                return false;
            }
        }

        return true;
    }

    PegasusCore::PegasusCore(sparta::TreeNode* core_tn, const PegasusCoreParameters* p) :
        sparta::Unit(core_tn),
        core_id_(p->core_id),
        num_harts_(p->num_harts),
        syscall_emulation_enabled_(p->enable_syscall_emulation),
        isa_string_(p->isa),
        supported_priv_modes_(initSupportedPrivilegeModes(p->priv)),
        xlen_(getXlenFromIsaString(isa_string_)),
        supported_rv64_extensions_(SUPPORTED_RV64_EXTS),
        supported_rv32_extensions_(SUPPORTED_RV32_EXTS),
        isa_file_path_(p->isa_file_path),
        uarch_file_path_(p->uarch_file_path),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"), isa_file_path_)),
        hypervisor_enabled_(extension_manager_.isEnabled("h")),
        inst_handlers_(syscall_emulation_enabled_)
    {
        // top.core*.hart*
        for (HartId hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            sparta::TreeNode* hart_tn = nullptr;
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            tns_to_delete_.emplace_back(
                hart_tn = new sparta::ResourceTreeNode(core_tn, hart_name, "harts", hart_idx,
                                                       "Hart State", &state_factory_));

            // Set XLEN
            hart_tn->getChildAs<sparta::ParameterBase>("params.xlen")
                ->setValueFromString(std::to_string(xlen_));

            // top.core*.hart*.fetch
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Fetch Unit", &fetch_factory_));

            // top.core*.hart*.translate
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));

            // top.core*.hart*.execute
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Execute Unit", &execute_factory_));

            // top.core*.hart*.exception
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));
        }

        sparta_assert(false == hypervisor_enabled_, "Hypervisor is not supported yet");
        sparta_assert(xlen_ == extension_manager_.getXLEN());

        extension_manager_.setISA(isa_string_);

        std::string unsupportedExt;
        if (!validateISAString_(unsupportedExt))
        {
            sparta_assert(false,
                          "ISA extension: " << unsupportedExt
                                            << " is not supported in isa_string: " << isa_string_);
        }
    }

    PegasusCore::~PegasusCore() {}

    void PegasusCore::boot()
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->boot();
        }
    }

    void PegasusCore::stopSim(const int64_t exit_code)
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->stopSim(exit_code);
        }
    }

    void PegasusCore::onBindTreeEarly_()
    {
        // Pegasus System (shared by all harts)
        auto root_tn = getContainer()->getParentAs<sparta::RootTreeNode>();
        system_ = root_tn->getChild("system")->getResourceAs<pegasus::PegasusSystem>();

        for (uint32_t hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            // Get PegasusState for each hart
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            const auto thread = getContainer()->getChild(hart_name);
            threads_.emplace(hart_idx, thread->getResourceAs<PegasusState*>());

            PegasusState* state = threads_.at(hart_idx);
            // This MUST be done before initializing Mavis
            state->setPegasusCore(this);
            state->setPc(system_->getStartingPc());
        }

        // Initialize Mavis
        DLOG("Initializing Mavis with ISA string " << isa_string_);

        mavis_ = std::make_unique<MavisType>(
            extension_manager_.constructMavis<
                PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>(
                getUArchFiles_(), mavis_uid_list_, {}, // annotation overrides
                {},                                    // inclusions
                {},                                    // exclusions
                PegasusInstAllocatorWrapper<PegasusInstAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(getContainer()))
                        ->inst_allocator),
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(getContainer()))
                        ->extractor_allocator,
                    this)));

        if (isCompressionEnabled())
        {
            setPcAlignment_(2);
        }
        else
        {
            setPcAlignment_(4);
        }
    }

    void PegasusCore::onBindTreeLate_()
    {
        auto root_tn = getContainer()->getParentAs<sparta::RootTreeNode>();
        system_call_emulator_ =
            root_tn->getChild("system_call_emulator")->getResourceAs<pegasus::SystemCallEmulator>();

        for (uint32_t hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            PegasusState* state = threads_.at(hart_idx);

            const auto workload_and_args = system_->getWorkloadAndArgs();
            if (false == workload_and_args.empty())
            {
                state->setupProgramStack(workload_and_args);
            }
        }
    }

    void PegasusCore::changeMavisContext()
    {
        extension_manager_.switchMavisContext(*mavis_.get());

        if (isCompressionEnabled())
        {
            setPcAlignment_(2);
        }
        else
        {
            setPcAlignment_(4);
        }
    }

    template <typename XLEN> uint32_t PegasusCore::getMisaExtFieldValue() const
    {
        uint32_t ext_val = 0;
        for (char ext = 'a'; ext <= 'z'; ++ext)
        {
            const std::string ext_str = std::string(1, ext);
            if (extension_manager_.isEnabled(ext_str))
            {
                ext_val |= 1 << getCsrBitRange<XLEN>(MISA, ext_str.c_str()).first;
            }
        }

        // FIXME: Assume both User and Supervisor mode are supported
        ext_val |= 1 << CSR::MISA::u::high_bit;
        ext_val |= 1 << CSR::MISA::s::high_bit;

        return ext_val;
    }

    template uint32_t PegasusCore::getMisaExtFieldValue<RV32>() const;
    template uint32_t PegasusCore::getMisaExtFieldValue<RV64>() const;

    mavis::FileNameListType PegasusCore::getUArchFiles_() const
    {
        const std::string xlen_str = std::to_string(xlen_);
        const std::string xlen_uarch_file_path = uarch_file_path_ + "/rv" + xlen_str + "/gen";
        if (xlen_ == 64)
        {
            const mavis::FileNameListType uarch_files = RV64_UARCH_JSON_LIST;
            return uarch_files;
        }
        else
        {
            const mavis::FileNameListType uarch_files = RV32_UARCH_JSON_LIST;
            return uarch_files;
        }
    }
} // namespace pegasus
