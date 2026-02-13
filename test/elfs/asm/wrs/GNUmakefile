
TEST_NAME := $(shell basename $(CURDIR))
MAKEFILE  := $(lastword $(MAKEFILE_LIST))
RISCV_ARCH_FLAGS := -march=rv64imafd_zicsr_zifencei_zihintpause_zalrsc_zawrs

ifndef ASM_INCLUDES
$(error Build this from the parent directory: "$(MAKE) ASM_TESTS=$(TEST_NAME) ..")
endif

SOURCES = $(wildcard *.s)
OBJECTS = $(SOURCES:.s=.o)
TARGETS = $(SOURCES:.s=.elf)

all : $(TARGETS)

%.elf: %.o $(MAKEFILE) $(COMMON_ASM)
	$(RV64_ASM_LINKER) -T $(ASM_INCLUDES)/main.ld -e main $< -o $@

%.o: %.s $(MAKEFILE) $(COMMON_ASM)
	$(RV64_ASM_COMPILER) $(RISCV_ARCH_FLAGS) -I$(ASM_INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGETS)
