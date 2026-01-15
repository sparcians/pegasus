#include "core/inst_handlers/v/RvvLoadStoreInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void
    RvvLoadStoreInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle32.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle64.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 64, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle64.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse32.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse64.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 64, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse64.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vlm.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsmComputeAddressHandler_<XLEN>,
                                          RvvLoadStoreInsts>(nullptr, "vlm.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsm.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsmComputeAddressHandler_<XLEN>,
                                          RvvLoadStoreInsts>(nullptr, "vsm.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vlse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse32.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse64.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 64, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse64.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse32.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse64.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 64, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse64.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 64, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vluxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vluxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vluxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vluxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vluxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vluxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vluxei64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 64, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vluxei64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vle8ff.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT, true>,
                RvvLoadStoreInsts>(nullptr, "vle8ff.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle16ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::UNIT, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle16ff.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle32ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::UNIT, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle32ff.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle64ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseComputeAddressHandler_<
                                              XLEN, 64, AddressingMode::UNIT, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle64ff.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 64, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsuxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsuxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsuxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsuxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsuxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsuxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsuxei64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 64, AddressingMode::IDX_UNORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsuxei64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vl1re8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 8>,
                                          RvvLoadStoreInsts>(nullptr, "vl1re8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl1re16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 16>,
                                          RvvLoadStoreInsts>(nullptr, "vl1re16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl1re32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 32>,
                                          RvvLoadStoreInsts>(nullptr, "vl1re32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl1re64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 64>,
                                          RvvLoadStoreInsts>(nullptr, "vl1re64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vl2re8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 8>,
                                          RvvLoadStoreInsts>(nullptr, "vl2re8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl2re16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 16>,
                                          RvvLoadStoreInsts>(nullptr, "vl2re16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl2re32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 32>,
                                          RvvLoadStoreInsts>(nullptr, "vl2re32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl2re64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 64>,
                                          RvvLoadStoreInsts>(nullptr, "vl2re64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vl4re8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 8>,
                                          RvvLoadStoreInsts>(nullptr, "vl4re8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl4re16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 16>,
                                          RvvLoadStoreInsts>(nullptr, "vl4re16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl4re32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 32>,
                                          RvvLoadStoreInsts>(nullptr, "vl4re32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl4re64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 64>,
                                          RvvLoadStoreInsts>(nullptr, "vl4re64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vl8re8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 8>,
                                          RvvLoadStoreInsts>(nullptr, "vl8re8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl8re16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 16>,
                                          RvvLoadStoreInsts>(nullptr, "vl8re16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl8re32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 32>,
                                          RvvLoadStoreInsts>(nullptr, "vl8re32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vl8re64.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, 64>,
                                          RvvLoadStoreInsts>(nullptr, "vl8re64.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vs1r.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, VLEN_MIN>, RvvLoadStoreInsts>(
                nullptr, "vs1r.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vs2r.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, VLEN_MIN>, RvvLoadStoreInsts>(
                nullptr, "vs2r.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vs4r.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, VLEN_MIN>, RvvLoadStoreInsts>(
                nullptr, "vs4r.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vs8r.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlsreComputeAddressHandler_<XLEN, VLEN_MIN>, RvvLoadStoreInsts>(
                nullptr, "vs8r.v", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvvLoadStoreInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, true>,
                                                    RvvLoadStoreInsts>(nullptr, "vle8.v",
                                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vle16.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vle32.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<64, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vle64.v",
                                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, false>,
                                                    RvvLoadStoreInsts>(nullptr, "vse8.v",
                                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vse16.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vse32.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<64, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vse64.v",
                                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vlm.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsmHandler_<true>,
                                                   RvvLoadStoreInsts>(nullptr, "vlm.v",
                                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsm.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsmHandler_<false>,
                                                   RvvLoadStoreInsts>(nullptr, "vsm.v",
                                                                      ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vlse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vlse8.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vlse16.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vlse32.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<64, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vlse64.v",
                                                                         ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vsse8.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, false>,
                                                      RvvLoadStoreInsts>(nullptr, "vsse16.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, false>,
                                                      RvvLoadStoreInsts>(nullptr, "vsse32.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<64, false>,
                                                      RvvLoadStoreInsts>(nullptr, "vsse64.v",
                                                                         ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vloxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                       RvvLoadStoreInsts>(nullptr, "vloxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vloxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vloxei32.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vloxei64.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vluxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                       RvvLoadStoreInsts>(nullptr, "vluxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vluxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vluxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vluxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vluxei32.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vluxei64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vluxei64.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vle8ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, true, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle8ff.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle16ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, true, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle16ff.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle32ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, true, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle32ff.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle64ff.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<64, true, true>,
                                          RvvLoadStoreInsts>(nullptr, "vle64ff.v",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsoxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                       RvvLoadStoreInsts>(nullptr, "vsoxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsoxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsoxei32.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsoxei64.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsuxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                       RvvLoadStoreInsts>(nullptr, "vsuxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsuxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsuxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsuxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsuxei32.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsuxei64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsuxei64.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vl1re8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<8, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vl1re8.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl1re16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<16, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl1re16.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl1re32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<32, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl1re32.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl1re64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<64, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl1re64.v",
                                                                          ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vl2re8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<8, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vl2re8.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl2re16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<16, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl2re16.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl2re32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<32, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl2re32.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl2re64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<64, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl2re64.v",
                                                                          ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vl4re8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<8, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vl4re8.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl4re16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<16, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl4re16.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl4re32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<32, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl4re32.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl4re64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<64, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl4re64.v",
                                                                          ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vl8re8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<8, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vl8re8.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl8re16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<16, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl8re16.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl8re32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<32, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl8re32.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vl8re64.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<64, true>,
                                                       RvvLoadStoreInsts>(nullptr, "vl8re64.v",
                                                                          ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vs1r.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<VLEN_MIN, false>,
                                          RvvLoadStoreInsts>(nullptr, "vs1r.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vs2r.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<VLEN_MIN, false>,
                                          RvvLoadStoreInsts>(nullptr, "vs2r.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vs4r.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<VLEN_MIN, false>,
                                          RvvLoadStoreInsts>(nullptr, "vs4r.v",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vs8r.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlsreHandler_<VLEN_MIN, false>,
                                          RvvLoadStoreInsts>(nullptr, "vs8r.v",
                                                             ActionTags::EXECUTE_TAG));
    }

    template void
    RvvLoadStoreInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void
    RvvLoadStoreInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvvLoadStoreInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvLoadStoreInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, RvvLoadStoreInsts::AddressingMode addrMode,
              bool ffirst>
    Action::ItrType RvvLoadStoreInsts::vlseComputeAddressHandler_(pegasus::PegasusState* state,
                                                                  Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig* config = inst->getVecConfig();
        const size_t eewb = elemWidth / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        Addr stride;
        if constexpr (addrMode == AddressingMode::UNIT)
        {
            stride = eewb;
        }
        else if constexpr (addrMode == AddressingMode::STRIDED)
        {
            stride = READ_INT_REG<XLEN>(state, inst->getRs2());
        }
        if (inst->getVM())
        {
            for (size_t i = config->getVSTART(); i < config->getVL(); ++i)
            {
                const bool throw_fault = ffirst && (i == 0);
                inst->getTranslationState()->makeRequest(rs1_val + i * stride, eewb, throw_fault);
            }
        }
        else // masked
        {
            const MaskElements mask_elems{state, config, pegasus::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                const auto i = mask_iter.getIndex();
                if (ffirst && i == 0)
                {
                    inst->getTranslationState()->makeRequest(rs1_val + i * stride, eewb, true);
                }
                else
                {
                    inst->getTranslationState()->makeRequest(rs1_val + i * stride, eewb);
                }
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvLoadStoreInsts::AddressingMode addrMode>
    Action::ItrType RvvLoadStoreInsts::vlseIdxComputeAddressHandler_(pegasus::PegasusState* state,
                                                                     Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVecConfig();
        const size_t sewb = vector_config->getSEW() / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVecConfig(), inst->getRs2()};

        if (inst->getVM())
        {
            for (auto stride : elems_vs2)
            {
                inst->getTranslationState()->makeRequest(rs1_val + stride.getVal(), sewb);
            }
        }
        else
        {
            const MaskElements mask_elems{state, inst->getVecConfig(), pegasus::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                inst->getTranslationState()->makeRequest(
                    rs1_val + elems_vs2.getElement(mask_iter.getIndex()).getVal(), sewb);
            }
        }

        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth>
    Action::ItrType RvvLoadStoreInsts::vlsreComputeAddressHandler_(pegasus::PegasusState* state,
                                                                   Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVecConfig();
        const size_t vl = vector_config->getVL();
        const size_t eewb = elemWidth / 8;
        const Addr stride = elemWidth / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        for (size_t i = vector_config->getVSTART(); i < vl; ++i)
        {
            inst->getTranslationState()->makeRequest(rs1_val + i * stride, eewb);
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvLoadStoreInsts::vlsmComputeAddressHandler_(pegasus::PegasusState* state,
                                                                  Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        constexpr size_t BYTESIZE = 8;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVecConfig();
        const size_t vl = (vector_config->getVL() + BYTESIZE - 1) / BYTESIZE;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        for (size_t i = vector_config->getVSTART(); i < vl; ++i)
        {
            inst->getTranslationState()->makeRequest(rs1_val + i, 1);
        }

        return ++action_it;
    }

    template <size_t elemWidth, bool isLoad, bool ffirst>
    Action::ItrType RvvLoadStoreInsts::vlseHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems{state, inst->getVecConfig(),
                                                  isLoad ? inst->getRd() : inst->getRs3()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto transtate = inst->getTranslationState();
                if (ffirst && !transtate->getNumResults())
                {
                    sparta_assert(!iter.getIndex(), "fault first should take trap on element 0.");
                    inst->getVecConfig()->setVL(iter.getIndex());
                    return;
                }
                if constexpr (isLoad)
                {
                    UintType<elemWidth> value = state->readMemory<UintType<elemWidth>>(
                        transtate->getResult(), MemAccessSource::INSTRUCTION);
                    elems.getElement(iter.getIndex()).setVal(value);
                }
                else
                {
                    UintType<elemWidth> value = elems.getElement(iter.getIndex()).getVal();
                    state->writeMemory<UintType<elemWidth>>(transtate->getResult(), value,
                                                            MemAccessSource::INSTRUCTION);
                }
                transtate->popResult();
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems.begin(), elems.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVecConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <bool isLoad>
    Action::ItrType RvvLoadStoreInsts::vlseIdxHandler_(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVecConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vlseHandler_<8, isLoad>(state, action_it);
            case 16:
                return vlseHandler_<16, isLoad>(state, action_it);
            case 32:
                return vlseHandler_<32, isLoad>(state, action_it);
            case 64:
                return vlseHandler_<64, isLoad>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <size_t elemWidth, bool isLoad>
    Action::ItrType RvvLoadStoreInsts::vlsreHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig* config = inst->getVecConfig();
        Elements<Element<elemWidth>, false> elems{state, config,
                                                  isLoad ? inst->getRd() : inst->getRs3()};

        for (auto iter = elems.begin(); iter != elems.end(); ++iter)
        {
            if constexpr (isLoad)
            {
                UintType<elemWidth> value = state->readMemory<UintType<elemWidth>>(
                    inst->getTranslationState()->getResult(), MemAccessSource::INSTRUCTION);
                elems.getElement(iter.getIndex()).setVal(value);
            }
            else
            {
                UintType<elemWidth> value = elems.getElement(iter.getIndex()).getVal();
                state->writeMemory<UintType<elemWidth>>(inst->getTranslationState()->getResult(),
                                                        value, MemAccessSource::INSTRUCTION);
            }
            inst->getTranslationState()->popResult();
        }

        return ++action_it;
    }

    template <bool isLoad>
    Action::ItrType RvvLoadStoreInsts::vlsmHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        constexpr size_t BYTESIZE = 8;
        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig* config = inst->getVecConfig();
        config->setLMUL(1 * 8);
        config->setVL((config->getVL() + BYTESIZE - 1) / BYTESIZE);
        Elements<Element<BYTESIZE>, false> elems{state, config,
                                                 isLoad ? inst->getRd() : inst->getRs3()};

        for (auto iter = elems.begin(); iter != elems.end(); ++iter)
        {
            if constexpr (isLoad)
            {
                UintType<BYTESIZE> value = state->readMemory<UintType<BYTESIZE>>(
                    inst->getTranslationState()->getResult(), MemAccessSource::INSTRUCTION);
                elems.getElement(iter.getIndex()).setVal(value);
            }
            else
            {
                UintType<BYTESIZE> value = elems.getElement(iter.getIndex()).getVal();
                state->writeMemory<UintType<BYTESIZE>>(inst->getTranslationState()->getResult(),
                                                       value, MemAccessSource::INSTRUCTION);
            }
            inst->getTranslationState()->popResult();
        }

        return ++action_it;
    }
} // namespace pegasus
