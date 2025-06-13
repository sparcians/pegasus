
TEST_NAME := $(shell basename $(CURDIR))
MAKEFILE := $(lastword $(MAKEFILE_LIST))

$(TEST_NAME).elf: $(TEST_NAME).c $(MAKEFILE)
	$(RV64_C_COMPILER) $(RISCV_ARCH_FLAGS) -static -DTIME -DN_RUNS=10000  $< -o $(@F)

clean:
	rm $(TEST_NAME).elf
