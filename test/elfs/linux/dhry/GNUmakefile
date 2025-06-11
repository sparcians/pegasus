
TEST_NAME := $(shell basename $(CURDIR))

$(TEST_NAME).elf: $(TEST_NAME).c $(MAKEFILE)
	$(RV64_C_COMPILER) $(RISCV_ARCH_FLAGS) -DTIME  $< -o $(@F)

clean:
	rm $(TEST_NAME).elf
