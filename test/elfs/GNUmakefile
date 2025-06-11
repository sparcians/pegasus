
all: asm linux
clean:
	$(MAKE) TARGET=clean asm linux

.PHONY: asm linux

asm:
	$(MAKE) -C asm $(TARGET)

linux:
	$(MAKE) -C linux $(TARGET)

install: asm linux
	@$(SHELL) -ec 'ELFS=`find . -name *.elf`; \
	 for elf in $$ELFS; do \
		echo Copying $$elf to ../sim/worklads/; \
		cp $$elf ../sim/workloads/; \
	 done'
