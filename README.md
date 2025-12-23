# Pegasus
A RISC-V Functional Model, but different...

![riscv-arch](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/kathlenemagnus/257907f4095b77a22da35df05f543a4a/raw/riscv-arch.json)
![tenstorrent](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/kathlenemagnus/257907f4095b77a22da35df05f543a4a/raw/tenstorrent.json)

                                   ,
                                   |`\
                                  /'_/_
                                ,'_/\_/\_                       ,
                              ,'_/\'_\_,/_                    ,'|
                            ,'_/\_'_ \_ \_/                _,-'_/
                          ,'_/'\_'_ \_ \'_,\           _,-'_,-/ \,
                        ,' /_\ _'_ \_ \'_,/       __,-'<_,' _,\_,/
                       ( (' )\/(_ \_ \'_,\   __--' _,-_/_,-',_/ _\
                        \_`\> 6` 7  \'_,/ ,-' _,-,'\,_'_ \,_/'_,\
                          \/-  _/ 7 '/ _,' _/'\_  \,_'_ \_ \'_,/
                           \_'/>   7'_/' _/' \_ '\,_'_ \_ \'_,\
                             >/  _ ,V  ,<  \__ '\,_'_ \_ \'_,/
                           /'_  ( )_)\/-,',__ '\,_'_,\_,\'_\
                          ( ) \_ \|_  `\_    \_,/'\,_'_,/'
                           \\_  \_\_)    `\_
                            \_)   >        `\_
                                 /  `,      |`\_
                                /    \     / \ `\
                               /   __/|   /  /  `\
                              (`  (   (` (_  \   /
                              /  ,/    |  /  /   \
                             / ,/      | /   \   `\_
                           _/_/        |/    /__/,_/
                          /_(         /_(

                     ____
                    |  _ \ ___  __ _  __ _ ___ _   _ ___
                    | |_) / _ \/ _` |/ _` / __| | | / __|
                    |  __/  __/ (_| | (_| \__ \ |_| \__ \
                    |_|   \___|\__, |\__,_|___/\__,_|___/
                               |___/

## What is Pegasus?

Pegasus is an open-source functional simulator, built using the Sparta
Modeling Framework, designed for the purpose of co-simulation, STF
trace generation, and application development.  The main drivers or
use cases are Design Verification and Execution-Driven Mode (EDM)
performance simulators like Olympia.

Pegasus can be used to execute instructions in a holistic fashion or
in partial stages.  For example, setting Pegasus to execute an
instruction at address 0x1000 actually entails many steps, especially
for memory operations:

1. Instruction Translation
   - Can take an exception: access permission, miss in the MMU, etc
   - Can invoke a TLB walk
1. Data fetch for the instruction
   - Can encounter an external exception
   - Can miss in cache
1. Decode
   - Can take an exception: illop, invalid opcode
1. Execute
   - Instruction address generation for memory accesses
   - Instruction translation for memory accesses: can take an exception
   - Instruction data access: can miss in the cache
   - Instruction execution itself: can take an exception
   - Redirection or sequential PC

In most, if not all, functional simulators developed in the industry
perform all of the operations above during a single call to `step()`.
Pegasus is different.  Pegasus can stop at any point in the flow above
and return control back to the caller.  For example, if the user wants
to run simulation until a load instruction is about to access memory,
the user can advance Pegasus until that point and then stop.
"Continuing" the simulator re-enters that access point.

In addition, with each full step of an instruction, an Event object is
returned detailing what the instruction did on its journey through
the stages.  This event can be used to “undo” or backup the simulator
as need be.  This allows out-of-order simulators like Olympia to have
multiple paths of execution through a binary (think branch
mispredictions).

Pegasus is still under development and more documentation and support
will come; stay tuned!

## Install Prerequisites

Install the following packages (tested with Ubuntu 24.04):

- (apt-get install cmake) cmake v3.22
- (apt-get install clang) Clang, Version: 14.0.0
- (apt-get install lld) LLD 18.1.8
- (apt-get install libboost-all-dev) boost 1.74.0
- (apt-get install libyaml-cpp-dev) YAML CPP 0.7.0
- (apt-get install rapidjson-dev) RapidJSON CPP 1.1.0
- (apt-get install libsqlite3-dev) SQLite3 3.37.2
- (apt-get install libhdf5-dev) HDF5 1.10.7
- (apt-get install clang-format) clang-format version 18.1.3

Required for the STF library:
- (apt-get install llvm) LLVM version 18.1.3
- (apt-get install pkg-config) pkg-config 1.8.1
- (apt-get install zstd libzstd-dev) Zstandard CLI (64-bit) v1.5.5

A Pegasus Conda environment is also available:
```
conda env create -f conda/pegasus_env.yaml
conda activate pegasus
```

## Install Sparta
Download and build sparta, map_v2.1:
```
git clone git@github.com:sparcians/map --recursive --branch map_v2.1
cd map/sparta
mkdir release
cd release
CC=$COMPILER CXX=$CXX_COMPILER cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/
sudo make -j$(nproc --all) install
```

If using Conda, you must install Sparta in the Pegasus Conda environment:
```
cmake --install . --prefix $CONDA_PREFIX
```

## Clone Pegasus and Build/Regress
```
git clone git@github.com:sparcians/pegasus.git --recursive
cd pegasus
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release -DSPARTA_SEARCH_DIR=/usr/local
make
make pegasus_regress
```
Alternatively, you can build Pegasus without an install of Sparta by providing a path
to your Sparta repo and Pegasus will build the Sparta library for
you. Since Sparta already has a make target named `regress`, the Pegasus
regression make target is renamed to `pegasus_regress`.

```
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release -DSPARTA_SEARCH_DIR=<full path to map/sparta>
make
make pegasus_regress
```

## Install Pegasus
```
cmake --install . --prefix <full install path>
```

## Validate
### RISC-V Unit Tests
The RISC-V unit tests ([riscv-software-src/riscv-tests](https://github.com/riscv-software-src/riscv-tests))
are a set of self-checking unit tests provided by the RISC-V Foundation. Follow the directions in
the [README](https://github.com/riscv-software-src/riscv-tests#readme) to build the tests.

```
cd pegasus/<build>/sim
python ../../scripts/RunArchTest.py --riscv-arch $RISCV_TESTS_PATH/isa/
```

### Tenstorrent
The Tenstorrent architectural tests ([tenstorrent/riscv_arch_tests](https://github.com/tenstorrent/riscv_arch_tests))
are a set of self-checking architectural tests provided by Tenstorrent. Follow the directions in the
[README](https://github.com/riscv-software-src/riscv-tests?tab=readme-ov-file#building-from-repository)
to build the tests. Alternatively, pre-built tests can be downloaded here: [Releases/v0.2.0+aligned-access](https://github.com/tenstorrent/riscv_arch_tests/releases/tag/v0.2.0%2Baligned-access)
```
cd pegasus/<build>/sim
python ../../scripts/RunArchTest.py --tenstorrent $TENSTORRENT_TESTS_PATH/bare_metal/user/
```

## Debug

To build Pegasus for debugging, Debug and FastDebug build types are available.
```
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=FastDebug
```
```
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Python IDE
See [Python IDE for Pegasus](IDE/README.md)

## RISC-V Profile Support
| RVA23U64 Mandatory Extensions | Status |
| :---------------------------- | :----: |
| **M** Integer multiplication and division. | :white_check_mark: |
| **A** Atomic instructions. | :white_check_mark: |
| **F** Single-precision floating-point instructions. | :white_check_mark: |
| **D** Double-precision floating-point instructions. | :white_check_mark: |
| **C** Compressed instructions. | :white_check_mark: |
| **B** Bit-manipulation instructions. | :white_check_mark: |
| **Zicsr** CSR instructions. These are implied by presence of F. | :white_check_mark: |
| **Zicntr** Base counters and timers. | :x: |
| **Zihpm** Hardware performance counters. | :x: |
| **Ziccif** Main memory regions with both the cacheability and coherence PMAs must support instruction fetch, and any instruction fetches of naturally aligned power-of-2 sizes up to min(ILEN,XLEN) (i.e., 32 bits for RVA23) are atomic. | :x: |
| **Ziccrse** Main memory regions with both the cacheability and coherence PMAs must support RsrvEventual. | :x: |
| **Ziccamoa** Main memory regions with both the cacheability and coherence PMAs must support all atomics in A. | :x: |
| **Zicclsm** Misaligned loads and stores to main memory regions with both the cacheability and coherence PMAs must be supported. | :x: |
| **Za64rs** Reservation sets are contiguous, naturally aligned, and a maximum of 64 bytes. | :x: |
| **Zihintpause** Pause hint. | :white_check_mark: |
| **Zic64b** Cache blocks must be 64 bytes in size, naturally aligned in the address space. | :x: |
| **Zicbom** Cache-block management instructions. | :white_check_mark: |
| **Zicbop** Cache-block prefetch instructions. | :white_check_mark: |
| **Zicboz** Cache-Block Zero Instructions. | :white_check_mark: |
| **Zfhmin** Half-precision floating-point. | :x: |
| **Zkt** Data-independent execution latency. | :x: |
| **V** Vector extension. | :x: |
| **Zvfhmin** Vector minimal half-precision floating-point. | :x: |
| **Zvbb** Vector basic bit-manipulation instructions. | :x: |
| **Zvkt** Vector data-independent execution latency. | :x: |
| **Zihintntl** Non-temporal locality hints. | :white_check_mark: |
| **Zicond** Integer conditional operations. | :white_check_mark: |
| **Zimop** may-be-operations. | :x: |
| **Zcmop** Compressed may-be-operations. | :x: |
| **Zcb** Additional compressed instructions. | :white_check_mark: |
| **Zfa** Additional floating-Point instructions. | :white_check_mark: |
| **Zawrs** Wait-on-reservation-set instructions. | :x: |
| **Supm** Pointer masking, with the execution environment providing a means to select PMLEN=0 and PMLEN=7 at minimum. | :x: |


| RVA23U64 Optional Extensions | Status |
| :---------------------------- | :----: |
| **Zvkng** Vector crypto NIST algorithms with GCM. | :x: |
| **Zvksg** Vector crypto ShangMi algorithms with GCM. | :x: |
| **Zabha** Byte and halfword atomic memory operations. | :white_check_mark: |
| **Zacas** Compare-and-Swap instructions. | :x: |
| **Ziccamoc** Main memory regions with both the cacheability and coherence PMAs must provide AMOCASQ level PMA support. | :x: |
| **Zvbc** Vector carryless multiplication. | :x: |
| **Zama16b** Misaligned loads, stores, and AMOs to main memory regions that do not cross a naturally aligned 16-byte boundary are atomic. | :x: |
| **Zfh** Scalar half-precision floating-point. | :x: |
| **Zbc** Scalar carryless multiply. | :x: |
| **Zicfilp** Landing Pads. | :x: |
| **Zicfiss** Shadow Stack. | :x: |
| **Zvfh** Vector half-precision floating-point. | :x: |
| **Zfbfmin** Scalar BF16 converts. | :x: |
| **Zvfbfmin** Vector BF16 converts. | :x: |
| **Zvfbfwma** Vector BF16 widening mul-add. | :x: |


| RVA23S64 Mandatory Extensions | Status |
| :---------------------------- | :----: |
| **Zifencei** Instruction-Fetch Fence. | :white_check_mark: |
| **Svbare** The satp mode Bare must be supported. | :white_check_mark: |
| **Sv39** Page-based 39-bit virtual-Memory system. | :white_check_mark: |
| **Svade** Page-fault exceptions are raised when a page is accessed when A bit is clear, or written when D bit is clear. | :x: |
| **Ssccptr** Main memory regions with both the cacheability and coherence PMAs must support hardware page-table reads. | :x: |
| **Sstvecd** stvec.MODE must be capable of holding the value 0 (Direct). When stvec.MODE=Direct, stvec.BASE must be capable of holding any valid four-byte-aligned address. | :x: |
| **Sstvala** stval must be written with the faulting virtual address for load, store, and instruction page-fault, access-fault, and misaligned exceptions, and for breakpoint exceptions other than those caused by execution of the EBREAK or C.EBREAK instructions. For virtual-instruction and illegal-instruction exceptions, stval must be written with the faulting instruction. | :white_check_mark: |
| **Sscounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in scounteren must be writable. | :x: |
| **Svpbmt** Page-based memory types
| **Svinval** Fine-grained address-translation cache invalidation. | :x: |
| **Svnapot** NAPOT translation contiguity. | :x: |
| **Sstc** supervisor-mode timer interrupts. | :x: |
| **Sscofpmf** count overflow and mode-based filtering. | :x: |
| **Ssnpm** Pointer masking, with senvcfg.PME and henvcfg.PME supporting, at minimum, settings PMLEN=0 and PMLEN=7. | :x: |
| **Ssu64xl** sstatus.UXL must be capable of holding the value 2 (i.e., UXLEN=64 must be supported). | :x: |
| **H** The hypervisor extension. | :x: |
| **Ssstateen** Supervisor-mode view of the state-enable extension. The supervisor-mode (sstateen10-3 and hypervisor-mode (hstateen0-3) state-enable registers must be provided. | :x: |
| **Shcounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in hcounteren
must be writable. | |
| **Shvstvala** vstval must be written in all cases described above for stval. | |
| **Shtvala** htval must be written with the faulting guest physical address in all circumstances permitted by the ISA. | |
| **Shvstvecd** vstvec.MODE must be capable of holding the value 0 (Direct). When vstvec.MODE=Direct, vstvec.BASE must be capable of holding any valid four-byte-aligned address. | |
| **Shvsatpa** All translation modes supported in satp must be supported in vsatp. | |
| **Shgatpa** For each supported virtual memory scheme SvNN supported in satp, the corresponding hgatp SvNNx4 mode must be supported. The hgatp mode Bare must also be supported. | :x: |


| RVA23S64 Optional Extensions | Status |
| :--------------------------- | :----: |
| **Sv48** Page-based 48-bit virtual-memory system. | :white_check_mark: |
| **Sv57** Page-based 57-bit virtual-memory system. | :white_check_mark: |
| **Zkr** Entropy CSR. | :x: |
| **Svadu** Hardware A/D bit updates. | :x: |
| **Sdtrig** Debug triggers. | :x: |
| **Ssstrict** No non-conforming extensions are present. Attempts to execute unimplemented opcodes or access unimplemented CSRs in the standard or reserved encoding spaces raises an illegal instruction exception that results in a contained trap to the supervisor-mode trap handler. | :x: |
| **Svvptc** Transitions from invalid to valid PTEs will be visible in bounded time without an explicit memory-management fence. | |
| **Sspm** Supervisor-mode pointer masking, with the supervisor execution environment providing a
means to select PMLEN=0 and PMLEN=7 at minimum. | |
