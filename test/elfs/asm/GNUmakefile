
ASM_TESTS ?= b_ext ecall nop uart

include ../common.mk

export ASM_INCLUDES := $(shell readlink -f ../common/)

all:
	@echo "Using $(RV64_ASM_COMPILER)"
	@for asm_test in $(ASM_TESTS); do \
		echo "Building $$asm_test..."; \
		$(MAKE) -C $$asm_test ASM_INCLUDES=$(ASM_INCLUDES); \
     done

clean:
	@for asm_test in $(ASM_TESTS); do \
		echo "Building $$asm_test..."; \
		$(MAKE) -C $$asm_test ASM_INCLUDES=$(ASM_INCLUDES) clean; \
	done
