
ifndef COMMON_MK
COMMON_MK := TRUE

RISCV_TOOL_SUITE ?= /usr/bin
DEFAULT_ASM ?= $(RISCV_TOOL_SUITE)/riscv64-unknown-elf-as
DEFAULT_CC  ?= $(RISCV_TOOL_SUITE)/riscv64-linux-gnu-gcc

ifndef RV_ASM
RV_ASM := $(DEFAULT_ASM)
endif

ifndef RV_CC
RV_CC := $(DEFAULT_CC)
endif

export RISCV_ARCH_FLAGS  := -march=rv64gcv_zba_zbb_zbc_zbs
export RV64_ASM_COMPILER := $(shell which $(RV_ASM))
export RV64_C_COMPILER   := $(shell which $(RV_CC))

ifeq ($(RV64_ASM_COMPILER),)
$(error "Could not find a valid ASM compiler.  Given $(RV_ASM)")
endif

ifeq ($(RV64_C_COMPILER),)
$(error "Could not find a valid C compiler. Given $(RV_CC)")
endif


endif
