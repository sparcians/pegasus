
all: asm linux
clean:
	$(MAKE) TARGET=clean asm linux

.PHONY: asm linux

asm:
	$(MAKE) -C asm $(TARGET)

linux:
	$(MAKE) -C linux $(TARGET)

install: asm linux
	@rm --preserve-root ../sim/workloads/*
	@$(SHELL) -ec 'ELFS=`find . -name *.elf`; \
	 for elf in $$ELFS; do \
		echo Copying $$elf to ../sim/workloads/; \
		cp $$elf ../sim/workloads/; \
	 done'
