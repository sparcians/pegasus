#include "cosim/CoSimEventReplayer.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "cosim/Event.hpp"
#include "sim/PegasusSim.hpp"
#include "core/PegasusCore.hpp"
#include "core/PegasusState.hpp"
#include "sparta/app/SimulationConfiguration.hpp"
#include "simdb/sqlite/DatabaseManager.hpp"
#include "simdb/utils/Compress.hpp"
#include "source/include/softfloat.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

namespace pegasus::cosim
{

    CoSimEventReplayer::CoSimEventReplayer(const std::string & db_file, const std::string & arch)
    {
        if (arch == "rv32")
        {
            reg_width_ = 8;
        }
        else if (arch == "rv64")
        {
            reg_width_ = 16;
        }
        else
        {
            throw sparta::SpartaException("Invalid arch; must be rv32 or rv64, not ") << arch;
        }

        db_mgr_.reset(new simdb::DatabaseManager(db_file, false /*not a new file*/));
        scheduler_.reset(new sparta::Scheduler);
        pegasus_sim_.reset(new PegasusSim(scheduler_.get()));

        // Figure out the total number of events on disk
        auto num_evts_query = db_mgr_->createQuery("CompressedEvents");
        num_evts_query->select("EndArchId", num_events_on_disk_);
        num_evts_query->orderBy("EndArchId", simdb::QueryOrder::DESC);

        auto result_set = num_evts_query->getResultSet();
        if (!result_set.getNextRecord())
        {
            throw sparta::SpartaException("Cannot run cosim event replayer - no events on disk!");
        }

        // Arch IDs are 0-based
        ++num_events_on_disk_;

        // Recreate the final ParameterTree config
        sim_config_.reset(new sparta::app::SimulationConfiguration);

        auto apply_db_ptree = [&](const char* table_name, sparta::ParameterTree & ptree)
        {
            auto ptree_query = db_mgr_->createQuery(table_name);

            std::string ptree_path, ptree_value;
            ptree_query->select("PTreePath", ptree_path);
            ptree_query->select("PTreeValue", ptree_value);

            auto ptree_results = ptree_query->getResultSet();
            while (ptree_results.getNextRecord())
            {
                ptree.set(ptree_path, ptree_value, false /*unrequired*/);
            }
        };

        apply_db_ptree("ArchParameterTree", sim_config_->getArchUnboundParameterTree());
        apply_db_ptree("ConfigParameterTree", sim_config_->getUnboundParameterTree());
        sim_config_->copyTreeNodeExtensionsFromArchAndConfigPTrees();

        // Configure PegasusSim
        pegasus_sim_->configure(0, nullptr, sim_config_.get());
        pegasus_sim_->buildTree();
        pegasus_sim_->configureTree();
        pegasus_sim_->finalizeTree();
        pegasus_sim_->finalizeFramework();

        // Cache replayers
        std::vector<sparta::TreeNode*> core_tns;
        pegasus_sim_->getRoot()->findChildren("core*", core_tns);
        checkpoint_replayers_.resize(core_tns.size());
        last_event_.resize(core_tns.size());
        for (CoreId core_id = 0; core_id < core_tns.size(); ++core_id)
        {
            auto core = core_tns[core_id]->getResourceAs<pegasus::PegasusCore*>();
            checkpoint_replayers_.at(core_id).resize(core->getNumThreads());
            last_event_.at(core_id).resize(core->getNumThreads());
            for (HartId hart_id = 0; hart_id < core->getNumThreads(); ++hart_id)
            {
                auto state = core->getPegasusState(hart_id);
                std::vector<sparta::TreeNode*> chkptr_arch_data_roots;
                chkptr_arch_data_roots.push_back(state->getContainer());
                chkptr_arch_data_roots.push_back(core->getSystem()->getContainer());

                auto replayer =
                    std::make_shared<CheckpointReplayer>(db_mgr_.get(), chkptr_arch_data_roots);
                checkpoint_replayers_.at(core_id).at(hart_id) = replayer;
            }
        }

        // No need to call PegasusState::boot(). The checkpoint replayer has already
        // loaded the memory/registers previously taken as the starting simulation
        // state.
    }

    const PegasusSim & CoSimEventReplayer::getPegasusSim() const { return *pegasus_sim_; }

    PegasusSim & CoSimEventReplayer::getPegasusSim() { return *pegasus_sim_; }

    bool CoSimEventReplayer::step(CoreId core_id, HartId hart_id)
    {
        if (next_arch_id_ + 1 > num_events_on_disk_)
        {
            return false;
        }

        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        auto event = recreateEventFromDisk_(next_arch_id_++, core_id, hart_id);
        last_event_.at(core_id).at(hart_id) = event;

        // Load memory/registers prior to applying the Event. Needed for things
        // that rely on CSRs e.g. updating the translation mode.
        auto replayer = checkpoint_replayers_.at(core_id).at(hart_id);
        replayer->step();

        if (reg_width_ == 8)
        {
            apply_<uint32_t>(*event, state);
        }
        else
        {
            apply_<uint64_t>(*event, state);
        }

        return true;
    }

    const Event* CoSimEventReplayer::getLastEvent(CoreId core_id, HartId hart_id) const
    {
        return last_event_.at(core_id).at(hart_id).get();
    }

    std::shared_ptr<Event>
    CoSimEventReplayer::recreateEventFromDisk_(uint64_t arch_id, CoreId core_id, HartId hart_id)
    {
        auto get_from_cache = [&]() -> Event*
        {
            if (cached_window_.empty())
            {
                return nullptr;
            }

            if (arch_id < cached_window_.front().getArchId()
                || arch_id > cached_window_.back().getArchId())
            {
                return nullptr;
            }

            auto idx = arch_id - cached_window_.front().getArchId();
            return &cached_window_.at(idx);
        };

        // See if the requested event is already in our cache
        if (auto evt = get_from_cache())
        {
            return std::make_shared<Event>(*evt);
        }

        std::vector<char> compressed_evts_bytes;

        auto query = db_mgr_->createQuery("CompressedEvents");
        query->addConstraintForUInt64("StartArchId", simdb::Constraints::LESS_EQUAL, arch_id);
        query->addConstraintForUInt64("EndArchId", simdb::Constraints::GREATER_EQUAL, arch_id);
        query->addConstraintForInt("CoreId", simdb::Constraints::EQUAL, (int)core_id);
        query->addConstraintForInt("HartId", simdb::Constraints::EQUAL, (int)hart_id);
        query->select("ZlibBlob", compressed_evts_bytes);

        auto result_set = query->getResultSet();
        result_set.getNextRecord();

        if (compressed_evts_bytes.empty())
        {
            return nullptr;
        }

        // "Undo" the pipeline transforms. Start by undoing zlib.
        std::vector<char> uncompressed_evts_bytes;
        simdb::decompressData(compressed_evts_bytes, uncompressed_evts_bytes);

        // Now undo boost::serialization
        namespace bio = boost::iostreams;
        bio::array_source src(uncompressed_evts_bytes.data(), uncompressed_evts_bytes.size());
        bio::stream<bio::array_source> is(src);
        boost::archive::binary_iarchive ia(is);

        // Get the EventList and cache it; the next requested event will usually be in the cache
        ia >> cached_window_;

        // If we got this far, the event uid must be within the returned list
        if (auto evt = get_from_cache())
        {
            return std::make_shared<Event>(*evt);
        }

        throw simdb::DBException("Internal error occurred. Cannot find event with arch ID ")
            << arch_id << ". Core " << core_id << ", hart " << hart_id << ", database '"
            << db_mgr_->getDatabaseFilePath() << "'.";
    }

    template <typename XLEN>
    void CoSimEventReplayer::apply_(const Event & reload_evt, PegasusState* state)
    {
        static_assert(std::is_same_v<XLEN, uint32_t> || std::is_same_v<XLEN, uint64_t>);

        // pc
        state->setPc(reload_evt.getNextPc());

        // priv mode
        state->setPrivMode(reload_evt.getNextPrivilegeMode(), state->getVirtualMode());

        // reservation
        auto hart_id = reload_evt.getHartId();
        if (reload_evt.getEndReservation().isValid())
        {
            state->getCore()->getReservation(hart_id) = reload_evt.getEndReservation();
        }
        else
        {
            state->getCore()->getReservation(hart_id).clearValid();
        }

        // softfloat
        softfloat_roundingMode = reload_evt.end_softfloat_flags_.softfloat_roundingMode;
        softfloat_detectTininess = reload_evt.end_softfloat_flags_.softfloat_detectTininess;
        softfloat_exceptionFlags = reload_evt.end_softfloat_flags_.softfloat_exceptionFlags;
        extF80_roundingPrecision = reload_evt.end_softfloat_flags_.extF80_roundingPrecision;

        // sim state
        auto sim_state = state->getSimState();
        sim_state->reset();
        sim_state->current_opcode = reload_evt.getOpcode();
        sim_state->current_uid = reload_evt.getSimStateCurrentUID();
        sim_state->sim_stopped = reload_evt.isLastEvent();
        sim_state->inst_count = reload_evt.getSimStateCurrentUID();
        if (sim_state->sim_stopped)
        {
            sim_state->workload_exit_code = reload_evt.getWorkloadExitCode();
            sim_state->test_passed = sim_state->workload_exit_code == 0;
        }

        // mmu mode / translation mode
        bool change_mmu_mode = false;
        change_mmu_mode |= reload_evt.curr_priv_ != reload_evt.next_priv_;
        change_mmu_mode |= reload_evt.curr_ldst_priv_ != reload_evt.next_ldst_priv_;
        if (!change_mmu_mode && reload_evt.inst_csr_ != std::numeric_limits<uint32_t>::max())
        {
            change_mmu_mode |= reload_evt.inst_csr_ == MSTATUS || reload_evt.inst_csr_ == SSTATUS
                               || reload_evt.inst_csr_ == VSSTATUS
                               || reload_evt.inst_csr_ == HSTATUS;
            change_mmu_mode |= reload_evt.inst_csr_ == SATP || reload_evt.inst_csr_ == VSATP
                               || reload_evt.inst_csr_ == HGATP;
        }

        if (change_mmu_mode)
        {
            state->updateTranslationMode<XLEN>(translate_types::TranslationStage::SUPERVISOR);
            state->updateTranslationMode<XLEN>(
                translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
            state->updateTranslationMode<XLEN>(translate_types::TranslationStage::GUEST);
        }

        // current exception
        state->setCurrentException(reload_evt.getExceptionCode());

        // x10 changes due to syscall
        if (reload_evt.ecall_x10_changes_.changed())
        {
            auto x10_value = reload_evt.ecall_x10_changes_.getX10After();
            if (state->getXlen() == 64)
            {
                WRITE_INT_REG<RV64>(state, 10, x10_value);
            }
            else
            {
                WRITE_INT_REG<RV32>(state, 10, static_cast<RV32>(x10_value));
            }
        }

        // enabled extensions
        std::vector<std::string> exts_to_enable;
        std::vector<std::string> exts_to_disable;
        const auto & ext_changes = reload_evt.extension_changes_;
        for (auto rit = ext_changes.rbegin(); rit != ext_changes.rend(); ++rit)
        {
            const auto & ext_info = *rit;
            const auto & ext_names = ext_info.extensions;
            const auto enabled = ext_info.enabled;

            if (enabled)
            {
                exts_to_enable.insert(exts_to_enable.end(), ext_names.begin(), ext_names.end());
            }
            else
            {
                exts_to_disable.insert(exts_to_disable.end(), ext_names.begin(), ext_names.end());
            }
        }

        exts_to_enable.erase(std::unique(exts_to_enable.begin(), exts_to_enable.end()),
                             exts_to_enable.end());
        exts_to_disable.erase(std::unique(exts_to_disable.begin(), exts_to_disable.end()),
                              exts_to_disable.end());

        if (!exts_to_enable.empty() || !exts_to_disable.empty())
        {
            auto & ext_mgr = state->getExtensionManager();
            ext_mgr.changeExtensions(exts_to_enable, exts_to_disable);
            state->changeMavisContext();
        }
    }

} // namespace pegasus::cosim
