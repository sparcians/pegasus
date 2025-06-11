
all: asm linux
clean:
	$(MAKE) TARGET=clean asm linux

.PHONY: asm linux

asm:
	$(MAKE) -C asm $(TARGET)

linux:
	$(MAKE) -C linux $(TARGET)
