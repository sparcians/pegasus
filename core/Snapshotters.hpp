#pragma once

//Core database headers
#include "simdb/ObjectManager.hpp"
#include "simdb/ObjectRef.hpp"
#include "simdb/TableRef.hpp"
#include "simdb/utils/ObjectQuery.hpp"
#include "simdb/utils/BlobHelpers.hpp"
#include "simdb/async/AsyncTaskEval.hpp"

//SQLite-specific headers
#include "simdb/impl/sqlite/SQLiteConnProxy.hpp"
#include "simdb/impl/sqlite/TransactionUtils.hpp"
#include "simdb/impl/sqlite/Errors.hpp"
#include <sqlite3.h>

namespace atlas
{

class InstSnapshotter : public simdb::WorkerTask
{
public:
    InstSnapshotter(simdb::ObjectManager* obj_mgr, int hart,
                    const std::string& rs1_name, uint64_t rs1_val,
                    const std::string& rs2_name, uint64_t rs2_val,
                    const std::string& rd_name, uint64_t rd_val_before, uint64_t rd_val_after,
                    uint64_t cosim_rd_val_after,
                    int has_imm, uint64_t imm,
                    const std::string& disasm,
                    const std::string& mnemonic,
                    uint64_t opcode, uint64_t pc, int priv, int result_code)
        : obj_mgr_(obj_mgr)
        , hart_(hart)
        , rs1_name_(rs1_name)
        , rs1_val_(rs1_val)
        , rs2_name_(rs2_name)
        , rs2_val_(rs2_val)
        , rd_name_(rd_name)
        , rd_val_before_(rd_val_before)
        , rd_val_after_(rd_val_after)
        , cosim_rd_val_after_(cosim_rd_val_after)
        , has_imm_(has_imm)
        , imm_(imm)
        , disasm_(disasm)
        , mnemonic_(mnemonic)
        , opcode_(opcode)
        , pc_(pc)
        , priv_(priv)
        , result_code_(result_code)
    {}

    void completeTask() override
    {
        auto tbl = obj_mgr_->getTable("Instructions");

        tbl->createObjectWithArgs("HartId", hart_,
                                    "Rs1", rs1_name_,
                                    "Rs1Val", rs1_val_,
                                    "Rs2", rs2_name_,
                                    "Rs2Val", rs2_val_,
                                    "Rd", rd_name_,
                                    "RdValBefore", rd_val_before_,
                                    "RdValAfter", rd_val_after_,
                                    "TruthRdValAfter", cosim_rd_val_after_,
                                    "HasImm", has_imm_,
                                    "Imm", imm_,
                                    "Disasm", disasm_,
                                    "Mnemonic", mnemonic_,
                                    "Opcode", opcode_,
                                    "PC", pc_,
                                    "Priv", priv_,
                                    "ResultCode", result_code_);
    }

private:
    simdb::ObjectManager* obj_mgr_;
    int hart_;
    std::string rs1_name_;
    uint64_t rs1_val_;
    std::string rs2_name_;
    uint64_t rs2_val_;
    std::string rd_name_;
    uint64_t rd_val_before_;
    uint64_t rd_val_after_;
    uint64_t cosim_rd_val_after_;
    int has_imm_;
    uint64_t imm_;
    std::string disasm_;
    std::string mnemonic_;
    uint64_t opcode_;
    uint64_t pc_;
    int priv_;
    int result_code_;
};

class CsrValuesSnapshotter : public simdb::WorkerTask
{
public:
    CsrValuesSnapshotter(simdb::ObjectManager* obj_mgr, int hart, uint64_t pc, const std::vector<std::tuple<std::string, uint64_t, uint64_t>> &all_csr_vals)
        : obj_mgr_(obj_mgr)
        , hart_(hart)
        , pc_(pc)
        , all_csr_vals_(all_csr_vals)
    {}

    void completeTask() override
    {
        auto tbl = obj_mgr_->getTable("PostInstCsrVals");
        for (const auto& tup : all_csr_vals_) {
            const auto& csr_name = std::get<0>(tup);
            const auto atlas_csr_val = std::get<1>(tup);
            const auto cosim_csr_val = std::get<2>(tup);

            tbl->createObjectWithArgs("HartId", hart_,
                                        "PC", pc_,
                                        "CsrName", csr_name,
                                        "AtlasCsrVal", atlas_csr_val,
                                        "CosimCsrVal", cosim_csr_val);
        }
    }

private:
    simdb::ObjectManager* obj_mgr_;
    int hart_;
    uint64_t pc_;
    std::vector<std::tuple<std::string, uint64_t, uint64_t>> all_csr_vals_;
};

} // namespace atlas
