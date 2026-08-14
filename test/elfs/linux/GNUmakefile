
LINUX_TESTS ?= dhry syscall_test fstatat_test threading

include ../common.mk

ifndef RV64_C_COMPILER
$(error "Do not have a RISCV Linux C compiler defined")
endif

all:
	@echo "Using $(RV64_C_COMPILER)"
	@for linux_test in $(LINUX_TESTS); do \
		echo "Building $$linux_test..."; \
		$(MAKE) -C $$linux_test; \
     done

clean:
	@for linux_test in $(LINUX_TESTS); do \
		echo "Cleaning $$linux_test..."; \
		$(MAKE) -C $$linux_test clean; \
	done
