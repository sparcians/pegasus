
TEST_NAME := $(shell basename $(CURDIR))
MAKEFILE  := $(lastword $(MAKEFILE_LIST))

ifndef ASM_INCLUDES
$(error Build this from the parent directory: "$(MAKE) ASM_TESTS=$(TEST_NAME) ..")
endif

$(TEST_NAME).elf: $(TEST_NAME).s $(MAKEFILE) $(COMMON_ASM)
	$(RV64_ASM_COMPILER) $(RISCV_ARCH_FLAGS) -I$(ASM_INCLUDES)  $< -o $(@F)

clean:
	rm $(TEST_NAME).elf
