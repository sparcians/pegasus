
TEST_NAME := $(shell basename $(CURDIR))
MAKEFILE := $(lastword $(MAKEFILE_LIST))

$(TEST_NAME).elf: $(TEST_NAME).c $(MAKEFILE)
	$(RV64_C_COMPILER) -g $(RISCV_ARCH_FLAGS) -static $< -o $(@F)

clean:
	rm $(TEST_NAME).elf
