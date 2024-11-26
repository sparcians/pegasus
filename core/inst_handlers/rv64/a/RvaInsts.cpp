#include "core/inst_handlers/rv64/a/RvaInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "amoadd.d",
            atlas::Action::createAction<&RvaInsts::amoadd_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoadd_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoadd.w",
            atlas::Action::createAction<&RvaInsts::amoadd_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoadd_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand.d",
            atlas::Action::createAction<&RvaInsts::amoand_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoand_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand.w",
            atlas::Action::createAction<&RvaInsts::amoand_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoand_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax.d",
            atlas::Action::createAction<&RvaInsts::amomax_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomax_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax.w",
            atlas::Action::createAction<&RvaInsts::amomax_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomax_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu.d",
            atlas::Action::createAction<&RvaInsts::amomaxu_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomaxu_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu.w",
            atlas::Action::createAction<&RvaInsts::amomaxu_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomaxu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin.d",
            atlas::Action::createAction<&RvaInsts::amomin_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomin_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin.w",
            atlas::Action::createAction<&RvaInsts::amomin_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomin_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu.d",
            atlas::Action::createAction<&RvaInsts::amominu_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amominu_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu.w",
            atlas::Action::createAction<&RvaInsts::amominu_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amominu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor.d",
            atlas::Action::createAction<&RvaInsts::amoor_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoor_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor.w",
            atlas::Action::createAction<&RvaInsts::amoor_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap.d",
            atlas::Action::createAction<&RvaInsts::amoswap_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoswap_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap.w",
            atlas::Action::createAction<&RvaInsts::amoswap_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoswap_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor.d",
            atlas::Action::createAction<&RvaInsts::amoxor_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoxor_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor.w",
            atlas::Action::createAction<&RvaInsts::amoxor_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoxor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr.d",
            atlas::Action::createAction<&RvaInsts::lr_d_64_compute_address_handler, RvaInsts>(
                nullptr, "lr_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr.w",
            atlas::Action::createAction<&RvaInsts::lr_w_64_compute_address_handler, RvaInsts>(
                nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc.d",
            atlas::Action::createAction<&RvaInsts::sc_d_64_compute_address_handler, RvaInsts>(
                nullptr, "sc_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc.w",
            atlas::Action::createAction<&RvaInsts::sc_w_64_compute_address_handler, RvaInsts>(
                nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));
    }

    void RvaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("amoadd.d",
                              atlas::Action::createAction<&RvaInsts::amoadd_d_64_handler, RvaInsts>(
                                  nullptr, "amoadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoadd.w",
                              atlas::Action::createAction<&RvaInsts::amoadd_w_64_handler, RvaInsts>(
                                  nullptr, "amoadd_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoand.d",
                              atlas::Action::createAction<&RvaInsts::amoand_d_64_handler, RvaInsts>(
                                  nullptr, "amoand_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoand.w",
                              atlas::Action::createAction<&RvaInsts::amoand_w_64_handler, RvaInsts>(
                                  nullptr, "amoand_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomax.d",
                              atlas::Action::createAction<&RvaInsts::amomax_d_64_handler, RvaInsts>(
                                  nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomax.w",
                              atlas::Action::createAction<&RvaInsts::amomax_w_64_handler, RvaInsts>(
                                  nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu.d", atlas::Action::createAction<&RvaInsts::amomaxu_d_64_handler, RvaInsts>(
                             nullptr, "amomaxu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu.w", atlas::Action::createAction<&RvaInsts::amomaxu_w_64_handler, RvaInsts>(
                             nullptr, "amomaxu_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomin.d",
                              atlas::Action::createAction<&RvaInsts::amomin_d_64_handler, RvaInsts>(
                                  nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomin.w",
                              atlas::Action::createAction<&RvaInsts::amomin_w_64_handler, RvaInsts>(
                                  nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu.d", atlas::Action::createAction<&RvaInsts::amominu_d_64_handler, RvaInsts>(
                             nullptr, "amominu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu.w", atlas::Action::createAction<&RvaInsts::amominu_w_64_handler, RvaInsts>(
                             nullptr, "amominu_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoor.d",
                              atlas::Action::createAction<&RvaInsts::amoor_d_64_handler, RvaInsts>(
                                  nullptr, "amoor_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoor.w",
                              atlas::Action::createAction<&RvaInsts::amoor_w_64_handler, RvaInsts>(
                                  nullptr, "amoor_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap.d", atlas::Action::createAction<&RvaInsts::amoswap_d_64_handler, RvaInsts>(
                             nullptr, "amoswap_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap.w", atlas::Action::createAction<&RvaInsts::amoswap_w_64_handler, RvaInsts>(
                             nullptr, "amoswap_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoxor.d",
                              atlas::Action::createAction<&RvaInsts::amoxor_d_64_handler, RvaInsts>(
                                  nullptr, "amoxor_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoxor.w",
                              atlas::Action::createAction<&RvaInsts::amoxor_w_64_handler, RvaInsts>(
                                  nullptr, "amoxor_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lr.d",
                              atlas::Action::createAction<&RvaInsts::lr_d_64_handler, RvaInsts>(
                                  nullptr, "lr_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lr.w",
                              atlas::Action::createAction<&RvaInsts::lr_w_64_handler, RvaInsts>(
                                  nullptr, "lr_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sc.d",
                              atlas::Action::createAction<&RvaInsts::sc_d_64_handler, RvaInsts>(
                                  nullptr, "sc_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sc.w",
                              atlas::Action::createAction<&RvaInsts::sc_w_64_handler, RvaInsts>(
                                  nullptr, "sc_w", ActionTags::EXECUTE_TAG));
    }

    ActionGroup* RvaInsts::amoadd_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoadd_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return lhs + RS2; })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomaxu_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomaxu_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return std::max(lhs,
        // uint32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoadd_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoadd_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return lhs + RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amominu_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amominu_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return std::min(lhs, RS2); }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amominu_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amominu_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return std::min(lhs,
        // uint32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoor_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoor_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return lhs | RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoxor_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoxor_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return lhs ^ RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoswap_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoswap_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t UNUSED lhs) { return RS2; })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.load_reserved<int64_t>(RS1));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoor_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoor_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return lhs | RS2; })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomaxu_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomaxu_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return std::max(lhs, RS2); }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoswap_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoswap_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t UNUSED lhs) { return RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomax_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomax_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](int32_t lhs) { return std::max(lhs,
        // int32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomin_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomin_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](int64_t lhs) { return std::min(lhs, int64_t(RS2));
        // }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoxor_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoxor_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return lhs ^ RS2; })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');

        // bool have_reservation = MMU.store_conditional<uint32_t>(RS1, RS2);

        // WRITE_RD(!have_reservation);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoand_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoand_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t lhs) { return lhs & RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomin_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomin_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](int32_t lhs) { return std::min(lhs,
        // int32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amoand_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoand_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return lhs & RS2; })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::amomax_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomax_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](int64_t lhs) { return std::max(lhs, int64_t(RS2));
        // }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(MMU.load_reserved<int32_t>(RS1));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;

        // bool have_reservation = MMU.store_conditional<uint64_t>(RS1, RS2);

        // WRITE_RD(!have_reservation);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas