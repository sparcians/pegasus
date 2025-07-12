#include <limits>
#include <type_traits>

#include "core/inst_handlers/v/RvvFloatInsts.hpp"
#include "core/inst_handlers/finsts_helpers.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvFloatInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // unary operations

        inst_handlers.emplace(
            "vfsqrt.v",
            atlas::Action::createAction<
                &RvvFloatInsts::vfunaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sqrt, f32_sqrt, f64_sqrt}>,
                RvvFloatInsts>(nullptr, "vfsqrt.v", ActionTags::EXECUTE_TAG));

        // binary operations

        inst_handlers.emplace(
            "vfadd.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfadd.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfadd.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfrsub.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfrbinaryHandler_<XLEN, FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfrsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmul.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmul.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfmul.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfdiv.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfdiv.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfdiv.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfdiv.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfrdiv.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfrbinaryHandler_<XLEN, FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfrdiv.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwadd.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmul.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfwmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmul.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfwmul.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.wv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.wf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.wf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.wv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.wf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.wf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfsgnj.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnj<float16_t>, fsgnj<float32_t>, fsgnj<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnj.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnj.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnj<float16_t>, fsgnj<float32_t>, fsgnj<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnj.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjn.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnjn<float16_t>, fsgnjn<float32_t>, fsgnjn<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjn.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjn.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnjn<float16_t>, fsgnjn<float32_t>, fsgnjn<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjn.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjx.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnjx<float16_t>, fsgnjx<float32_t>, fsgnjx<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjx.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjx.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnjx<float16_t>, fsgnjx<float32_t>, fsgnjx<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjx.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmfeq.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_eq, f32_eq, f64_eq}>,
                RvvFloatInsts>(nullptr, "vmfeq.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfeq.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_eq, f32_eq, f64_eq}>,
                RvvFloatInsts>(nullptr, "vmfeq.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfne.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fne<float16_t>, fne<float32_t>, fne<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfne.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfne.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fne<float16_t>, fne<float32_t>, fne<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfne.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmflt.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_lt, f32_lt, f64_lt}>,
                RvvFloatInsts>(nullptr, "vmflt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmflt.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_lt, f32_lt, f64_lt}>,
                RvvFloatInsts>(nullptr, "vmflt.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfle.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_le, f32_le, f64_le}>,
                RvvFloatInsts>(nullptr, "vmfle.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfle.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_le, f32_le, f64_le}>,
                RvvFloatInsts>(nullptr, "vmfle.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfgt.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fgt<float16_t>, fgt<float32_t>, fgt<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfgt.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfgt.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vmfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fge<float16_t>, fge<float32_t>, fge<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfgt.vf", ActionTags::EXECUTE_TAG));

        // ternary operations

        auto maccWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(src2, src1, dst).v; };
        inst_handlers.emplace(
            "vfmacc.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmacc.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfmacc.vf", ActionTags::EXECUTE_TAG));

        auto nmaccWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(fnegate(src2), src1, fnegate(dst)).v; };
        inst_handlers.emplace(
            "vfnmacc.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfnmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmacc.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfnmacc.vf", ActionTags::EXECUTE_TAG));

        auto msacWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(src2, src1, fnegate(dst)).v; };
        inst_handlers.emplace(
            "vfmsac.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmsac.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfmsac.vf", ActionTags::EXECUTE_TAG));

        auto nmsacWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(fnegate(src2), src1, dst).v; };
        inst_handlers.emplace(
            "vfnmsac.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmsac.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsac.vf", ActionTags::EXECUTE_TAG));

        auto maddWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(src1, dst, src2).v; };
        inst_handlers.emplace(
            "vfmadd.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    maddWrapper>,
                RvvFloatInsts>(nullptr, "vfmadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmadd.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    maddWrapper>,
                RvvFloatInsts>(nullptr, "vfmadd.vf", ActionTags::EXECUTE_TAG));

        auto nmaddWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(fnegate(src1), dst, fnegate(src2)).v; };
        inst_handlers.emplace(
            "vfnmadd.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaddWrapper>,
                RvvFloatInsts>(nullptr, "vfnmadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmadd.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaddWrapper>,
                RvvFloatInsts>(nullptr, "vfnmadd.vf", ActionTags::EXECUTE_TAG));

        auto msubWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(src1, dst, fnegate(src2)).v; };
        inst_handlers.emplace(
            "vfmsub.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    msubWrapper>,
                RvvFloatInsts>(nullptr, "vfmsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmsub.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    msubWrapper>,
                RvvFloatInsts>(nullptr, "vfmsub.vf", ActionTags::EXECUTE_TAG));

        auto nmsubWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        { return mulAdd(fnegate(src1), dst, src2).v; };
        inst_handlers.emplace(
            "vfnmsub.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsubWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmsub.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsubWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsub.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwmacc.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfwmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmacc.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfwmacc.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwnmacc.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwnmacc.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmacc.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwmsac.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfwmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmsac.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfwmsac.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwnmsac.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwnmsac.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfternaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmsac.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmin.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fmin<float16_t>, fmin<float32_t>, fmin<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmin.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmin.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fmin<float16_t>, fmin<float32_t>, fmin<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmin.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmax.vv",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fmax<float16_t>, fmax<float32_t>, fmax<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmax.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmax.vf",
            atlas::Action::createAction<
                &RvvFloatInsts::vfbinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fmax<float16_t>, fmax<float32_t>, fmax<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmax.vf", ActionTags::EXECUTE_TAG));
    }

    template void RvvFloatInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFloatInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vfunaryHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<
            Element<opMode.src2 == RvvFloatInsts::OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
            false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<
            Element<opMode.dst == RvvFloatInsts::OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
            false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                elems_vd.getElement(index).setVal(func(elems_vs2.getElement(index).getVal()));
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    /*
        template <typename XLEN, RvvFloatInsts::FloatFuncs funcs>
        Action::ItrType RvvFloatInsts::vfunaryHandler_(atlas::AtlasState* state,
                                                       Action::ItrType action_it)
        {
            VectorConfig* vector_config = state->getVectorConfig();
            switch (vector_config->getSEW())
            {
                case 16:
                    return vfunaryHelper<XLEN, 16, [](auto src2)
                                         { return funcs.f16(float16_t{src2}).v; }>(state,
       action_it); break; case 32: return vfunaryHelper<XLEN, 32, [](auto src2) { return
       funcs.f32(float32_t{src2}).v; }>(state, action_it); break; case 64: return
       vfunaryHelper<XLEN, 64, [](auto src2) { return funcs.f64(float64_t{src2}).v; }>(state,
       action_it); break; default: sparta_assert(false, "Unsupported SEW value"); break;
            }
            return ++action_it;
        }
    */

    template <typename XLEN, RvvFloatInsts::OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfunaryHandler_(atlas::AtlasState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfunaryHelper<XLEN, 16, opMode, [](auto src2)
                                     {
                                         using Traits = FuncTraits<decltype(funcs.f16)>;
                                         using ReturnType = typename Traits::ReturnType;
                                         using ArgType =
                                             std::tuple_element_t<0, typename Traits::ArgsTuple>;
                                         if constexpr (std::is_integral_v<ReturnType>)
                                         {
                                             return funcs.f16(ArgType{src2});
                                         }
                                         else
                                         {
                                             return funcs.f16(ArgType{src2}).v;
                                         }
                                     }>(state, action_it);
                break;
            case 32:
                return vfunaryHelper<XLEN, 32, opMode, [](auto src2)
                                     {
                                         using Traits = FuncTraits<decltype(funcs.f32)>;
                                         using ReturnType = typename Traits::ReturnType;
                                         using ArgType =
                                             std::tuple_element_t<0, typename Traits::ArgsTuple>;
                                         if constexpr (std::is_integral_v<ReturnType>)
                                         {
                                             return funcs.f32(ArgType{src2});
                                         }
                                         else
                                         {
                                             return funcs.f32(ArgType{src2}).v;
                                         }
                                     }>(state, action_it);
                break;
            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W
                              && opMode.src2
                                     != OperandMode::Mode::W) // neither narrowing nor widening
                {
                    return vfunaryHelper<XLEN, 64, opMode, [](auto src2)
                                         {
                                             using Traits = FuncTraits<decltype(funcs.f64)>;
                                             using ReturnType = typename Traits::ReturnType;
                                             using ArgType =
                                                 std::tuple_element_t<0,
                                                                      typename Traits::ArgsTuple>;
                                             if constexpr (std::is_integral_v<ReturnType>)
                                             {
                                                 return funcs.f64(ArgType{src2});
                                             }
                                             else
                                             {
                                                 return funcs.f64(ArgType{src2}).v;
                                             }
                                         }>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vfbinaryHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<
            Element<opMode.src2 == RvvFloatInsts::OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
            false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<
            Element<opMode.dst == RvvFloatInsts::OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
            false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                typename decltype(elems_vd)::ElemType::ValueType value = 0;
                if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::V)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal());
                }
                else if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::F)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())));
                }
                elems_vd.getElement(index).setVal(value);
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfbinaryHandler_(atlas::AtlasState* state,
                                                    Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfbinaryHelper<XLEN, 16, opMode, [](auto src2, auto src1)
                                      {
                                          if constexpr (opMode.dst == OperandMode::Mode::W)
                                          {
                                              return funcs.f32(float32_t{src2}, float32_t{src1}).v;
                                          }
                                          else
                                          {
                                              return funcs.f16(float16_t{src2}, float16_t{src1}).v;
                                          }
                                      }>(state, action_it);
                break;
            case 32:
                return vfbinaryHelper<XLEN, 32, opMode, [](auto src2, auto src1)
                                      {
                                          if constexpr (opMode.dst == OperandMode::Mode::W)
                                          {
                                              return funcs.f64(float64_t{src2}, float64_t{src1}).v;
                                          }
                                          else
                                          {
                                              return funcs.f32(float32_t{src2}, float32_t{src1}).v;
                                          }
                                      }>(state, action_it);
                break;
            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W)
                {
                    return vfbinaryHelper<XLEN, 64, opMode, [](auto src2, auto src1) {
                        return funcs.f64(float64_t{src2}, float64_t{src1}).v;
                    }>(state, action_it);
                    break;
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfrbinaryHandler_(atlas::AtlasState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfbinaryHelper<XLEN, 16,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f16(float16_t{src1}, float16_t{src2}).v;
                                      }>(state, action_it);
                break;
            case 32:
                return vfbinaryHelper<XLEN, 32,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f32(float32_t{src1}, float32_t{src2}).v;
                                      }>(state, action_it);
                break;
            case 64:
                return vfbinaryHelper<XLEN, 64,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f64(float64_t{src1}, float64_t{src2}).v;
                                      }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vmfbinaryHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setBit(func(elems_vs2.getElement(index).getVal(),
                                                           elems_vs1.getElement(index).getVal()));
                }
                else if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::F)
                {
                    elems_vd.getElement(index).setBit(
                        func(elems_vs2.getElement(index).getVal(),
                             static_cast<UintType<elemWidth>>(
                                 READ_FP_REG<RV64>(state, inst->getRs1()))));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vmfbinaryHandler_(atlas::AtlasState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfbinaryHelper<XLEN, 16, opMode, [](auto src2, auto src1) {
                    return funcs.f16(float16_t{src2}, float16_t{src1});
                }>(state, action_it);
                break;
            case 32:
                return vfbinaryHelper<XLEN, 32, opMode, [](auto src2, auto src1) {
                    return funcs.f32(float32_t{src2}, float32_t{src1});
                }>(state, action_it);
                break;
            case 64:
                return vfbinaryHelper<XLEN, 64, opMode, [](auto src2, auto src1) {
                    return funcs.f64(float64_t{src2}, float64_t{src1});
                }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vfternaryHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<
            Element<opMode.dst == RvvFloatInsts::OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
            false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                auto elem_vd = elems_vd.getElement(index);
                typename Element<elemWidth>::ValueType value = 0;
                if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::V)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal(), elem_vd.getVal());
                }
                else if constexpr (opMode.src1 == RvvFloatInsts::OperandMode::Mode::F)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())),
                        elem_vd.getVal());
                }
                elem_vd.setVal(value);
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode, auto funcWrapper>
    Action::ItrType RvvFloatInsts::vfternaryHandler_(atlas::AtlasState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 16:
                return vfternaryHelper<XLEN, 16, opMode, [](auto src2, auto src1, auto dst)
                                       {
                                           if constexpr (opMode.dst == OperandMode::Mode::W)
                                           {
                                               return funcWrapper.template operator()<f32_mulAdd>(
                                                   float32_t{src2}, float32_t{src1},
                                                   float32_t{dst});
                                           }
                                           else
                                           {
                                               return funcWrapper.template operator()<f16_mulAdd>(
                                                   float16_t{src2}, float16_t{src1},
                                                   float16_t{dst});
                                           }
                                       }>(state, action_it);
                break;
            case 32:
                return vfternaryHelper<XLEN, 32, opMode, [](auto src2, auto src1, auto dst)
                                       {
                                           if constexpr (opMode.dst == OperandMode::Mode::W)
                                           {
                                               return funcWrapper.template operator()<f64_mulAdd>(
                                                   float64_t{src2}, float64_t{src1},
                                                   float64_t{dst});
                                           }
                                           else
                                           {
                                               return funcWrapper.template operator()<f32_mulAdd>(
                                                   float32_t{src2}, float32_t{src1},
                                                   float32_t{dst});
                                           }
                                       }>(state, action_it);
                break;
            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W)
                {
                    return vfternaryHelper<XLEN, 64, opMode, [](auto src2, auto src1, auto dst)
                                           {
                                               return funcWrapper.template operator()<f64_mulAdd>(
                                                   float64_t{src2}, float64_t{src1},
                                                   float64_t{dst});
                                           }>(state, action_it);
                    break;
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }
} // namespace atlas
