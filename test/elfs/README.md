Build directions:
```
mkdir build
cd build
cmake .. -DRISCV_TOOLCHAIN=<path to the install of RISCV toolsuite>
make
```
Basic notes:
- If the binary is expected to be run with system emulation enabled,
  the ELF _must_ contain `_end` symbol preferrably at the end of the
  ELF or it's eventual data segment
