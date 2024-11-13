#include "core/inst_handlers/rv64/i/RviInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RviInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "lb", atlas::Action::createAction<&RviInsts::lb_64_compute_address_handler, RviInsts>(
                      nullptr, "lb", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lbu", atlas::Action::createAction<&RviInsts::lbu_64_compute_address_handler, RviInsts>(
                       nullptr, "lbu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "ld", atlas::Action::createAction<&RviInsts::ld_64_compute_address_handler, RviInsts>(
                      nullptr, "ld", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lh", atlas::Action::createAction<&RviInsts::lh_64_compute_address_handler, RviInsts>(
                      nullptr, "lh", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lhu", atlas::Action::createAction<&RviInsts::lhu_64_compute_address_handler, RviInsts>(
                       nullptr, "lhu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lw", atlas::Action::createAction<&RviInsts::lw_64_compute_address_handler, RviInsts>(
                      nullptr, "lw", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lwu", atlas::Action::createAction<&RviInsts::lwu_64_compute_address_handler, RviInsts>(
                       nullptr, "lwu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sb", atlas::Action::createAction<&RviInsts::sb_64_compute_address_handler, RviInsts>(
                      nullptr, "sb", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sd", atlas::Action::createAction<&RviInsts::sd_64_compute_address_handler, RviInsts>(
                      nullptr, "sd", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sh", atlas::Action::createAction<&RviInsts::sh_64_compute_address_handler, RviInsts>(
                      nullptr, "sh", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sw", atlas::Action::createAction<&RviInsts::sw_64_compute_address_handler, RviInsts>(
                      nullptr, "sw", ActionTags::COMPUTE_ADDR_TAG));
    }

    void RviInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("add",
                              atlas::Action::createAction<&RviInsts::add_64_handler, RviInsts>(
                                  nullptr, "add", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("addi",
                              atlas::Action::createAction<&RviInsts::addi_64_handler, RviInsts>(
                                  nullptr, "addi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("addiw",
                              atlas::Action::createAction<&RviInsts::addiw_64_handler, RviInsts>(
                                  nullptr, "addiw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("addw",
                              atlas::Action::createAction<&RviInsts::addw_64_handler, RviInsts>(
                                  nullptr, "addw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("and",
                              atlas::Action::createAction<&RviInsts::and_64_handler, RviInsts>(
                                  nullptr, "and", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("andi",
                              atlas::Action::createAction<&RviInsts::andi_64_handler, RviInsts>(
                                  nullptr, "andi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("auipc",
                              atlas::Action::createAction<&RviInsts::auipc_64_handler, RviInsts>(
                                  nullptr, "auipc", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("beq",
                              atlas::Action::createAction<&RviInsts::beq_64_handler, RviInsts>(
                                  nullptr, "beq", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("bge",
                              atlas::Action::createAction<&RviInsts::bge_64_handler, RviInsts>(
                                  nullptr, "bge", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("bgeu",
                              atlas::Action::createAction<&RviInsts::bgeu_64_handler, RviInsts>(
                                  nullptr, "bgeu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("blt",
                              atlas::Action::createAction<&RviInsts::blt_64_handler, RviInsts>(
                                  nullptr, "blt", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("bltu",
                              atlas::Action::createAction<&RviInsts::bltu_64_handler, RviInsts>(
                                  nullptr, "bltu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("bne",
                              atlas::Action::createAction<&RviInsts::bne_64_handler, RviInsts>(
                                  nullptr, "bne", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cdiscard.d.l1",
            atlas::Action::createAction<&RviInsts::cdiscard_d_l1_64_handler, RviInsts>(
                nullptr, "cdiscard_d_l1", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cflush.d.l1", atlas::Action::createAction<&RviInsts::cflush_d_l1_64_handler, RviInsts>(
                               nullptr, "cflush_d_l1", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cflush.i.l1", atlas::Action::createAction<&RviInsts::cflush_i_l1_64_handler, RviInsts>(
                               nullptr, "cflush_i_l1", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("dret",
                              atlas::Action::createAction<&RviInsts::dret_64_handler, RviInsts>(
                                  nullptr, "dret", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("ebreak",
                              atlas::Action::createAction<&RviInsts::ebreak_64_handler, RviInsts>(
                                  nullptr, "ebreak", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("ecall",
                              atlas::Action::createAction<&RviInsts::ecall_64_handler, RviInsts>(
                                  nullptr, "ecall", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fence",
                              atlas::Action::createAction<&RviInsts::fence_64_handler, RviInsts>(
                                  nullptr, "fence", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("jal",
                              atlas::Action::createAction<&RviInsts::jal_64_handler, RviInsts>(
                                  nullptr, "jal", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("jalr",
                              atlas::Action::createAction<&RviInsts::jalr_64_handler, RviInsts>(
                                  nullptr, "jalr", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lb", atlas::Action::createAction<&RviInsts::lb_64_handler, RviInsts>(
                                        nullptr, "lb", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lbu",
                              atlas::Action::createAction<&RviInsts::lbu_64_handler, RviInsts>(
                                  nullptr, "lbu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("ld", atlas::Action::createAction<&RviInsts::ld_64_handler, RviInsts>(
                                        nullptr, "ld", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lh", atlas::Action::createAction<&RviInsts::lh_64_handler, RviInsts>(
                                        nullptr, "lh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lhu",
                              atlas::Action::createAction<&RviInsts::lhu_64_handler, RviInsts>(
                                  nullptr, "lhu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("li", atlas::Action::createAction<&RviInsts::li_64_handler, RviInsts>(
                                        nullptr, "li", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lui",
                              atlas::Action::createAction<&RviInsts::lui_64_handler, RviInsts>(
                                  nullptr, "lui", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lw", atlas::Action::createAction<&RviInsts::lw_64_handler, RviInsts>(
                                        nullptr, "lw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lwu",
                              atlas::Action::createAction<&RviInsts::lwu_64_handler, RviInsts>(
                                  nullptr, "lwu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mret",
                              atlas::Action::createAction<&RviInsts::mret_64_handler, RviInsts>(
                                  nullptr, "mret", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mv", atlas::Action::createAction<&RviInsts::mv_64_handler, RviInsts>(
                                        nullptr, "mv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("nop",
                              atlas::Action::createAction<&RviInsts::nop_64_handler, RviInsts>(
                                  nullptr, "nop", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("or", atlas::Action::createAction<&RviInsts::or_64_handler, RviInsts>(
                                        nullptr, "or", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("ori",
                              atlas::Action::createAction<&RviInsts::ori_64_handler, RviInsts>(
                                  nullptr, "ori", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sb", atlas::Action::createAction<&RviInsts::sb_64_handler, RviInsts>(
                                        nullptr, "sb", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sd", atlas::Action::createAction<&RviInsts::sd_64_handler, RviInsts>(
                                        nullptr, "sd", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sfence.vma", atlas::Action::createAction<&RviInsts::sfence_vma_64_handler, RviInsts>(
                              nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sh", atlas::Action::createAction<&RviInsts::sh_64_handler, RviInsts>(
                                        nullptr, "sh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sll",
                              atlas::Action::createAction<&RviInsts::sll_64_handler, RviInsts>(
                                  nullptr, "sll", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("slli",
                              atlas::Action::createAction<&RviInsts::slli_64_handler, RviInsts>(
                                  nullptr, "slli", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("slliw",
                              atlas::Action::createAction<&RviInsts::slliw_64_handler, RviInsts>(
                                  nullptr, "slliw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sllw",
                              atlas::Action::createAction<&RviInsts::sllw_64_handler, RviInsts>(
                                  nullptr, "sllw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("slt",
                              atlas::Action::createAction<&RviInsts::slt_64_handler, RviInsts>(
                                  nullptr, "slt", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("slti",
                              atlas::Action::createAction<&RviInsts::slti_64_handler, RviInsts>(
                                  nullptr, "slti", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sltiu",
                              atlas::Action::createAction<&RviInsts::sltiu_64_handler, RviInsts>(
                                  nullptr, "sltiu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sltu",
                              atlas::Action::createAction<&RviInsts::sltu_64_handler, RviInsts>(
                                  nullptr, "sltu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sra",
                              atlas::Action::createAction<&RviInsts::sra_64_handler, RviInsts>(
                                  nullptr, "sra", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("srai",
                              atlas::Action::createAction<&RviInsts::srai_64_handler, RviInsts>(
                                  nullptr, "srai", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sraiw",
                              atlas::Action::createAction<&RviInsts::sraiw_64_handler, RviInsts>(
                                  nullptr, "sraiw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sraw",
                              atlas::Action::createAction<&RviInsts::sraw_64_handler, RviInsts>(
                                  nullptr, "sraw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sret",
                              atlas::Action::createAction<&RviInsts::sret_64_handler, RviInsts>(
                                  nullptr, "sret", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("srl",
                              atlas::Action::createAction<&RviInsts::srl_64_handler, RviInsts>(
                                  nullptr, "srl", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("srli",
                              atlas::Action::createAction<&RviInsts::srli_64_handler, RviInsts>(
                                  nullptr, "srli", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("srliw",
                              atlas::Action::createAction<&RviInsts::srliw_64_handler, RviInsts>(
                                  nullptr, "srliw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("srlw",
                              atlas::Action::createAction<&RviInsts::srlw_64_handler, RviInsts>(
                                  nullptr, "srlw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sub",
                              atlas::Action::createAction<&RviInsts::sub_64_handler, RviInsts>(
                                  nullptr, "sub", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("subw",
                              atlas::Action::createAction<&RviInsts::subw_64_handler, RviInsts>(
                                  nullptr, "subw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sw", atlas::Action::createAction<&RviInsts::sw_64_handler, RviInsts>(
                                        nullptr, "sw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("uret",
                              atlas::Action::createAction<&RviInsts::uret_64_handler, RviInsts>(
                                  nullptr, "uret", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("wfi",
                              atlas::Action::createAction<&RviInsts::wfi_64_handler, RviInsts>(
                                  nullptr, "wfi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("xor",
                              atlas::Action::createAction<&RviInsts::xor_64_handler, RviInsts>(
                                  nullptr, "xor", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("xori",
                              atlas::Action::createAction<&RviInsts::xori_64_handler, RviInsts>(
                                  nullptr, "xori", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
