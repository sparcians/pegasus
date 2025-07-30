#include <limits>

#include "core/inst_handlers/v/RvvIntegerInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvIntegerInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vmv.v.v",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src1 = OperandMode::Mode::V}>,
                RvvIntegerInsts>(nullptr, "vmv.v.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.v.x",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src1 = OperandMode::Mode::X}>,
                RvvIntegerInsts>(nullptr, "vmv.v.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.v.i",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src1 = OperandMode::Mode::I}>,
                RvvIntegerInsts>(nullptr, "vmv.v.i", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vadd.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsub.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::minus>,
                RvvIntegerInsts>(nullptr, "vsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::minus>,
                RvvIntegerInsts>(nullptr, "vsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::virsubHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                   .src2 = OperandMode::Mode::V,
                                                                   .src1 = OperandMode::Mode::X}>,
                RvvIntegerInsts>(nullptr, "vrsub.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrsub.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::virsubHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                   .src2 = OperandMode::Mode::V,
                                                                   .src1 = OperandMode::Mode::I}>,
                RvvIntegerInsts>(nullptr, "vrsub.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwadd.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwaddu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsub.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsubu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vzext.vf2", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<2, false>,
                                                       RvvIntegerInsts>(nullptr, "vzext.vf2",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsext.vf2", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<2, true>,
                                                       RvvIntegerInsts>(nullptr, "vsext.vf2",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vzext.vf4", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<4, false>,
                                                       RvvIntegerInsts>(nullptr, "vzext.vf4",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsext.vf4", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<4, true>,
                                                       RvvIntegerInsts>(nullptr, "vsext.vf4",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vzext.vf8", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<8, false>,
                                                       RvvIntegerInsts>(nullptr, "vzext.vf8",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsext.vf8", pegasus::Action::createAction<&RvvIntegerInsts::viextHandler_<8, true>,
                                                       RvvIntegerInsts>(nullptr, "vsext.vf8",
                                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vadc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::V},
                                                 true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::X},
                                                 true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vim",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::I},
                                                 true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vim", ActionTags::EXECUTE_TAG));

        auto carryFn = []<typename T>(T a, T b, T c)
        { return a >= std::numeric_limits<T>::max() - b - c; };

        inst_handlers.emplace(
            "vmadc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::V},
                                                  true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vim",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::I},
                                                  true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vim", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::V},
                                                  false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::I},
                                                  false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vi", ActionTags::EXECUTE_TAG));

        auto borrowFn = []<typename T>(T a, T b, T c)
        { return (b + c == std::numeric_limits<T>::max()) || (a < b + c); };

        inst_handlers.emplace(
            "vmsbc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::V},
                                                  true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::V},
                                                  false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vand.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vor.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vxor.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsll.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Sll>,
                RvvIntegerInsts>(nullptr, "vsll.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsll.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Sll>,
                RvvIntegerInsts>(nullptr, "vsll.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsll.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, Sll>,
                RvvIntegerInsts>(nullptr, "vsll.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsrl.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vsrl.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsrl.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vsrl.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsrl.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vsrl.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsra.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vsra.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsra.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vsra.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsra.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vsra.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vnsrl.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vnsrl.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnsrl.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vnsrl.wx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnsrl.wi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, Srl>,
                RvvIntegerInsts>(nullptr, "vnsrl.wi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vnsra.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vnsra.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnsra.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vnsra.wx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnsra.wi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::W,
                                                               .src1 = OperandMode::Mode::I},
                                                   false, Sra>,
                RvvIntegerInsts>(nullptr, "vnsra.wi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmseq.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsne.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsltu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsltu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmslt.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               true, std::less>,
                RvvIntegerInsts>(nullptr, "vmslt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmslt.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               true, std::less>,
                RvvIntegerInsts>(nullptr, "vmslt.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsleu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsle.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgtu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgtu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgtu.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgt.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::V},
                                               true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgt.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::X},
                                               true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgt.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN,
                                               OperandMode{.dst = OperandMode::Mode::V,
                                                           .src2 = OperandMode::Mode::V,
                                                           .src1 = OperandMode::Mode::I},
                                               true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmax.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, Max>,
                RvvIntegerInsts>(nullptr, "vmax.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmax.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, Max>,
                RvvIntegerInsts>(nullptr, "vmax.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmaxu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Max>,
                RvvIntegerInsts>(nullptr, "vmaxu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmaxu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Max>,
                RvvIntegerInsts>(nullptr, "vmaxu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmin.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, Min>,
                RvvIntegerInsts>(nullptr, "vmin.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmin.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, Min>,
                RvvIntegerInsts>(nullptr, "vmin.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vminu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Min>,
                RvvIntegerInsts>(nullptr, "vminu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vminu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Min>,
                RvvIntegerInsts>(nullptr, "vminu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmul.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmul.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vmul.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulh.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Mulh>,
                RvvIntegerInsts>(nullptr, "vmulh.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulh.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Mulh>,
                RvvIntegerInsts>(nullptr, "vmulh.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulhu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Mulhu>,
                RvvIntegerInsts>(nullptr, "vmulhu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulhu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Mulhu>,
                RvvIntegerInsts>(nullptr, "vmulhu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulhsu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, Mulhsu>,
                RvvIntegerInsts>(nullptr, "vmulhsu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmulhsu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, Mulhsu>,
                RvvIntegerInsts>(nullptr, "vmulhsu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vdivu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::divides>,
                RvvIntegerInsts>(nullptr, "vdivu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vdivu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::divides>,
                RvvIntegerInsts>(nullptr, "vdivu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vdiv.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::divides>,
                RvvIntegerInsts>(nullptr, "vdiv.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vdiv.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::divides>,
                RvvIntegerInsts>(nullptr, "vdiv.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vremu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::modulus>,
                RvvIntegerInsts>(nullptr, "vremu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vremu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::modulus>,
                RvvIntegerInsts>(nullptr, "vremu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrem.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::modulus>,
                RvvIntegerInsts>(nullptr, "vrem.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrem.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::modulus>,
                RvvIntegerInsts>(nullptr, "vrem.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwmul.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vwmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmul.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   true, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vwmul.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmulu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vwmulu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmulu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viBinaryHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::W,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false, std::multiplies>,
                RvvIntegerInsts>(nullptr, "vwmulu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmulsu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viwmulsuHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::W,
                                                                     .src2 = OperandMode::Mode::V,
                                                                     .src1 = OperandMode::Mode::V}>,
                RvvIntegerInsts>(nullptr, "vwmulsu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmulsu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viwmulsuHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::W,
                                                                     .src2 = OperandMode::Mode::V,
                                                                     .src1 = OperandMode::Mode::X}>,
                RvvIntegerInsts>(nullptr, "vwmulsu.vx", ActionTags::EXECUTE_TAG));

        auto macc = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return sext<T2, T1>(src2) * sext<T2, T1>(src1) + dst; };

        auto maccu = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return zext<T2, T1>(src2) * zext<T2, T1>(src1) + dst; };

        auto maccsu = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return zext<T2, T1>(src2) * sext<T2, T1>(src1) + dst; };

        auto maccus = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return sext<T2, T1>(src2) * zext<T2, T1>(src1) + dst; };

        auto nmsac = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return -(sext<T2, T1>(src2) * sext<T2, T1>(src1)) + dst; };

        auto madd = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return sext<T2, T1>(src1) * dst + sext<T2, T1>(src2); };

        auto nmsub = []<typename T2, typename T1>(T1 src2, T1 src1, T2 dst) -> T2
        { return -(sext<T2, T1>(src1) * dst) + sext<T2, T1>(src2); };

        inst_handlers.emplace(
            "vmacc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    macc>,
                RvvIntegerInsts>(nullptr, "vmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmacc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    macc>,
                RvvIntegerInsts>(nullptr, "vmacc.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnmsac.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    nmsac>,
                RvvIntegerInsts>(nullptr, "vnmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnmsac.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    nmsac>,
                RvvIntegerInsts>(nullptr, "vnmsac.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadd.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    madd>,
                RvvIntegerInsts>(nullptr, "vmadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadd.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    madd>,
                RvvIntegerInsts>(nullptr, "vmadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnmsub.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    nmsub>,
                RvvIntegerInsts>(nullptr, "vnmsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnmsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::V,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    nmsub>,
                RvvIntegerInsts>(nullptr, "vnmsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwmaccu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    maccu>,
                RvvIntegerInsts>(nullptr, "vwmaccu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmaccu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    maccu>,
                RvvIntegerInsts>(nullptr, "vwmaccu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmacc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    macc>,
                RvvIntegerInsts>(nullptr, "vwmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmacc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    macc>,
                RvvIntegerInsts>(nullptr, "vwmacc.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmaccsu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::V},
                                                    maccsu>,
                RvvIntegerInsts>(nullptr, "vwmaccsu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmaccsu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    maccsu>,
                RvvIntegerInsts>(nullptr, "vwmaccsu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwmaccus.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viTernaryHandler_<XLEN,
                                                    OperandMode{.dst = OperandMode::Mode::W,
                                                                .src2 = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X},
                                                    maccus>,
                RvvIntegerInsts>(nullptr, "vwmaccus.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmerge.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimergeHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                    .src2 = OperandMode::Mode::V,
                                                                    .src1 = OperandMode::Mode::V}>,
                RvvIntegerInsts>(nullptr, "vmerge.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmerge.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimergeHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                    .src2 = OperandMode::Mode::V,
                                                                    .src1 = OperandMode::Mode::X}>,
                RvvIntegerInsts>(nullptr, "vmerge.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmerge.vim",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vimergeHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                    .src2 = OperandMode::Mode::V,
                                                                    .src1 = OperandMode::Mode::I}>,
                RvvIntegerInsts>(nullptr, "vmerge.vim", ActionTags::EXECUTE_TAG));
    }

    template void RvvIntegerInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvIntegerInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType vimvlHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        using ValueType = typename decltype(elems_vd)::ElemType::ValueType;
        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            if constexpr (opMode.src1 == OperandMode::Mode::V)
            {
                elems_vd.getElement(index).setVal(elems_vs1.getElement(index).getVal());
            }
            else if constexpr (opMode.src1 == OperandMode::Mode::X)
            {
                elems_vd.getElement(index).setVal(
                    static_cast<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
            }
            else // opMode.src1 == OperandMode::Mode::I
            {
                elems_vd.getElement(index).setVal(static_cast<ValueType>(inst->getImmediate()));
            }
        }
        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvIntegerInsts::vimvHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vimvlHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return vimvlHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return vimvlHelper<XLEN, 32, opMode>(state, action_it);

            case 64:
                return vimvlHelper<XLEN, 64, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode,
              template <typename> typename FunctorT, typename T>
    Action::ItrType viBinaryHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        FunctorT<T> functor{};
        using R = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                                   sext<T>(elems_vs1.getElement(index).getVal()))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                                   zext<T>(elems_vs1.getElement(index).getVal()))));
                    }
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(static_cast<R>(
                            functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                    sext<T>(READ_INT_REG<XLEN>(state, inst->getRs1())))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(static_cast<R>(
                            functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                    zext<T>(READ_INT_REG<XLEN>(state, inst->getRs1())))));
                    }
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                                   sext<T>(inst->getImmediate()))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                                   zext<T>(inst->getImmediate()))));
                    }
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viBinaryHandler_(PegasusState* state,
                                                      Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viBinaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int16_t, int8_t>>(state, action_it);

                case 16:
                    return viBinaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int32_t, int16_t>>(state, action_it);

                case 32:
                    return viBinaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int64_t, int32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viBinaryHelper<XLEN, 64, opMode, FunctorTemp, int64_t>(state,
                                                                                      action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viBinaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint16_t, uint8_t>>(state, action_it);

                case 16:
                    return viBinaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint32_t, uint16_t>>(state, action_it);

                case 32:
                    return viBinaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint64_t, uint32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viBinaryHelper<XLEN, 64, opMode, FunctorTemp, uint64_t>(state,
                                                                                       action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType virsubHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<UintType<elemWidth>>(READ_INT_REG<XLEN>(state, inst->getRs1()))
                        - elems_vs2.getElement(index).getVal());
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<UintType<elemWidth>>(inst->getImmediate())
                        - elems_vs2.getElement(index).getVal());
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvIntegerInsts::virsubHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return virsubHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return virsubHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return virsubHelper<XLEN, 32, opMode>(state, action_it);

            case 64:
                return virsubHelper<XLEN, 64, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType viwmulsuHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<2 * elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                         inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(
                        sext<UintType<2 * elemWidth>>(elems_vs2.getElement(index).getVal())
                        * zext<UintType<2 * elemWidth>>(elems_vs1.getElement(index).getVal()));
                }
                else // opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        sext<UintType<2 * elemWidth>>(elems_vs2.getElement(index).getVal())
                        * zext<UintType<2 * elemWidth>>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvIntegerInsts::viwmulsuHandler_(PegasusState* state,
                                                      Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viwmulsuHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return viwmulsuHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return viwmulsuHelper<XLEN, 32, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool hasMaskOp, typename Functor>
    Action::ItrType viacsbHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), pegasus::V0};
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        using ValueType = typename decltype(elems_vd)::ElemType::ValueType;

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            typename Element<elemWidth>::ValueType value = 0;
            if constexpr (opMode.src1 == OperandMode::Mode::V)
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal());
            }
            else if constexpr (opMode.src1 == OperandMode::Mode::X)
            {
                value =
                    Functor()(elems_vs2.getElement(index).getVal(),
                              static_cast<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
            }
            else // opMode.src1 == OperandMode::Mode::I
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<ValueType>(inst->getImmediate()));
            }
            if constexpr (hasMaskOp)
            {
                value = Functor()(value, static_cast<ValueType>(elems_v0.getBit(index)));
            }
            elems_vd.getElement(index).setVal(value);
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool hasMaskOp,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viacsbHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viacsbHelper<XLEN, 8, opMode, hasMaskOp, FunctorTemp<uint8_t>>(state,
                                                                                      action_it);

            case 16:
                return viacsbHelper<XLEN, 16, opMode, hasMaskOp, FunctorTemp<uint16_t>>(state,
                                                                                        action_it);

            case 32:
                return viacsbHelper<XLEN, 32, opMode, hasMaskOp, FunctorTemp<uint32_t>>(state,
                                                                                        action_it);

            case 64:
                return viacsbHelper<XLEN, 64, opMode, hasMaskOp, FunctorTemp<uint64_t>>(state,
                                                                                        action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool hasMaskOp, auto detectFunc>
    Action::ItrType vmiacsbHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), pegasus::V0};
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            auto a = elems_vs2.getElement(index).getVal();
            decltype(a) b = 0, c = 0;
            if constexpr (opMode.src1 == OperandMode::Mode::V)
            {
                b = elems_vs1.getElement(index).getVal();
            }
            else if constexpr (opMode.src1 == OperandMode::Mode::X)
            {
                b = static_cast<decltype(b)>(READ_INT_REG<XLEN>(state, inst->getRs1()));
            }
            else // opMode.src1 == OperandMode::Mode::I
            {
                b = static_cast<decltype(b)>(inst->getImmediate());
            }
            if constexpr (hasMaskOp)
            {
                c = static_cast<decltype(b)>(elems_v0.getBit(index));
            }
            elems_vd.getElement(index).setVal(detectFunc(a, b, c));
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFunc>
    Action::ItrType RvvIntegerInsts::vmiacsbHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmiacsbHelper<XLEN, 8, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 16:
                return vmiacsbHelper<XLEN, 16, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 32:
                return vmiacsbHelper<XLEN, 32, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 64:
                return vmiacsbHelper<XLEN, 64, opMode, hasMaskOp, detectFunc>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, typename Functor>
    Action::ItrType vmicHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                size_t index = iter.getIndex();
                bool result = false;
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    result = Functor()(elems_vs2.getElement(index).getVal(),
                                       elems_vs1.getElement(index).getVal());
                }
                else if (opMode.src1 == OperandMode::Mode::X)
                {
                    result = Functor()(elems_vs2.getElement(index).getVal(),
                                       READ_INT_REG<XLEN>(state, inst->getRs1()));
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    result = Functor()(elems_vs2.getElement(index).getVal(), inst->getImmediate());
                }
                if (result)
                {
                    elems_vd.setBit(index);
                }
                else
                {
                    elems_vd.clearBit(index);
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::vmicHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<int8_t>>(state, action_it);

                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<int16_t>>(state, action_it);

                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<int32_t>>(state, action_it);

                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<int64_t>>(state, action_it);

                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<uint8_t>>(state, action_it);

                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<uint16_t>>(state, action_it);

                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<uint32_t>>(state, action_it);

                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state, action_it);

                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType vimergeHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        const UintType<elemWidth> x =
            opMode.src1 == OperandMode::Mode::X
                ? static_cast<UintType<elemWidth>>(READ_INT_REG<XLEN>(state, inst->getRs1()))
            : opMode.src1 == OperandMode::Mode::I
                ? static_cast<UintType<elemWidth>>(inst->getImmediate())
                : 0;
        const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};

        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            if (mask_elems.getBit(index))
            {
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(elems_vs1.getElement(index).getVal());
                }
                else
                {
                    elems_vd.getElement(index).setVal(x);
                }
            }
            else
            {
                elems_vd.getElement(index).setVal(elems_vs2.getElement(index).getVal());
            }
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvIntegerInsts::vimergeHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vimergeHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return vimergeHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return vimergeHelper<XLEN, 32, opMode>(state, action_it);

            case 64:
                return vimergeHelper<XLEN, 64, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <size_t elemWidthRd, size_t elemWidthRs, bool isSigned>
    Action::ItrType viextHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidthRs>, false> elems_vs2{state, state->getVectorConfig(),
                                                        inst->getRs2()};
        Elements<Element<elemWidthRd>, false> elems_vd{state, state->getVectorConfig(),
                                                       inst->getRd()};
        using ValueType = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                if constexpr (isSigned)
                {
                    elems_vd.getElement(index).setVal(
                        sext<ValueType>(elems_vs2.getElement(index).getVal()));
                }
                else
                {
                    elems_vd.getElement(index).setVal(elems_vs2.getElement(index).getVal());
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <size_t fracVal, bool isSigned>
    Action::ItrType RvvIntegerInsts::viextHandler_(pegasus::PegasusState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                if constexpr (fracVal == 2)
                {
                    return viextHelper<16, 8, isSigned>(state, action_it);
                }
                else
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                break;
            case 32:
                if constexpr (fracVal == 2 || fracVal == 4)
                {
                    return viextHelper<32, 32 / fracVal, isSigned>(state, action_it);
                }
                else
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                break;
            case 64:
                if constexpr (fracVal == 2 || fracVal == 4 || fracVal == 8)
                {
                    return viextHelper<64, 64 / fracVal, isSigned>(state, action_it);
                }
                else
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType viTernaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        using ValueType = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                auto elem_vd = elems_vd.getElement(index);
                ValueType value = 0;
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal(), elem_vd.getVal());
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_INT_REG<XLEN>(state, inst->getRs1())),
                        elem_vd.getVal());
                }
                elem_vd.setVal(value);
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, auto func>
    Action::ItrType RvvIntegerInsts::viTernaryHandler_(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 8:
                return viTernaryHelper<XLEN, 8, opMode, func>(state, action_it);

            case 16:
                return viTernaryHelper<XLEN, 16, opMode, func>(state, action_it);

            case 32:
                return viTernaryHelper<XLEN, 32, opMode, func>(state, action_it);

            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W)
                {
                    return viTernaryHelper<XLEN, 64, opMode, func>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }
} // namespace pegasus
