#include "core/inst_handlers/rv64/m/RvmInsts.hpp"
<<<<<<< HEAD
=======
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/ActionGroup.hpp"
>>>>>>> main
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
namespace atlas
{
    void RvmInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("div",
                              atlas::Action::createAction<&RvmInsts::div_64_handler, RvmInsts>(
                                  nullptr, "div", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divu",
                              atlas::Action::createAction<&RvmInsts::divu_64_handler, RvmInsts>(
                                  nullptr, "divu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divuw",
                              atlas::Action::createAction<&RvmInsts::divuw_64_handler, RvmInsts>(
                                  nullptr, "divuw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divw",
                              atlas::Action::createAction<&RvmInsts::divw_64_handler, RvmInsts>(
                                  nullptr, "divw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mul",
                              atlas::Action::createAction<&RvmInsts::mul_64_handler, RvmInsts>(
                                  nullptr, "mul", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulh",
                              atlas::Action::createAction<&RvmInsts::mulh_64_handler, RvmInsts>(
                                  nullptr, "mulh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulhsu",
                              atlas::Action::createAction<&RvmInsts::mulhsu_64_handler, RvmInsts>(
                                  nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulhu",
                              atlas::Action::createAction<&RvmInsts::mulhu_64_handler, RvmInsts>(
                                  nullptr, "mulhu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulw",
                              atlas::Action::createAction<&RvmInsts::mulw_64_handler, RvmInsts>(
                                  nullptr, "mulw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("rem",
                              atlas::Action::createAction<&RvmInsts::rem_64_handler, RvmInsts>(
                                  nullptr, "rem", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remu",
                              atlas::Action::createAction<&RvmInsts::remu_64_handler, RvmInsts>(
                                  nullptr, "remu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remuw",
                              atlas::Action::createAction<&RvmInsts::remuw_64_handler, RvmInsts>(
                                  nullptr, "remuw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remw",
                              atlas::Action::createAction<&RvmInsts::remw_64_handler, RvmInsts>(
                                  nullptr, "remw", ActionTags::EXECUTE_TAG));
    }



    ActionGroup* RvmInsts::remu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // reg_t lhs = zext_xlen(RS1);
        // reg_t rhs = zext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(sext_xlen(RS1));
        // else
        //     WRITE_RD(sext_xlen(lhs % rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::div_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // sreg_t lhs = sext_xlen(RS1);
        // sreg_t rhs = sext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else if (lhs == INT64_MIN && rhs == -1)
        //     WRITE_RD(lhs);
        // else
        //     WRITE_RD(sext_xlen(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::rem_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // sreg_t lhs = sext_xlen(RS1);
        // sreg_t rhs = sext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(lhs);
        // else if (lhs == INT64_MIN && rhs == -1)
        //     WRITE_RD(0);
        // else
        //     WRITE_RD(sext_xlen(lhs % rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::divw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // sreg_t lhs = sext32(RS1);
        // sreg_t rhs = sext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else
        //     WRITE_RD(sext32(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::remuw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // reg_t lhs = zext32(RS1);
        // reg_t rhs = zext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(sext32(lhs));
        // else
        //     WRITE_RD(sext32(lhs % rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::mulh_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // if (xlen == 64)
        //     WRITE_RD(mulh(RS1, RS2));
        // else
        //     WRITE_RD(sext32((sext32(RS1) * sext32(RS2)) >> 32));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::divuw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // reg_t lhs = zext32(RS1);
        // reg_t rhs = zext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else
        //     WRITE_RD(sext32(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::mulw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // require_rv64;
        // WRITE_RD(sext32(RS1 * RS2));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::mulhu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // if (xlen == 64)
        //     WRITE_RD(mulhu(RS1, RS2));
        // else
        //     WRITE_RD(sext32(((uint64_t)(uint32_t)RS1 * (uint64_t)(uint32_t)RS2) >> 32));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::remw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // sreg_t lhs = sext32(RS1);
        // sreg_t rhs = sext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(lhs);
        // else
        //     WRITE_RD(sext32(lhs % rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::divu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // reg_t lhs = zext_xlen(RS1);
        // reg_t rhs = zext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else
        //     WRITE_RD(sext_xlen(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::mul_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // WRITE_RD(sext_xlen(RS1 * RS2));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvmInsts::mulhsu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // if (xlen == 64)
        //     WRITE_RD(mulhsu(RS1, RS2));
        // else
        //     WRITE_RD(sext32((sext32(RS1) * reg_t((uint32_t)RS2)) >> 32));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas