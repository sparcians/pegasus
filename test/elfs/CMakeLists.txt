cmake_minimum_required(VERSION 3.27.6)

project(riscv_assembly_tests ASM)

# Unset these variables to prevent include of Mac SDK
set(CMAKE_OSX_SYSROOT "/")
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

if(NOT RISCV_TOOLCHAIN)
  message(FATAL_ERROR "You must supply a -DRISCV_TOOLCHAIN=/path")
else()
  message("-- Using RISCV_TOOLCHAIN: ${RISCV_TOOLCHAIN}")
  if(EXISTS ${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-as)
    set(CMAKE_ASM_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-as")
    set(CMAKE_C_COMPILER   "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-gcc")
    set(CMAKE_CXX_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-g++")
    set(CMAKE_LINKER       "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-ld")
  elseif(EXISTS ${RISCV_TOOLCHAIN}//bin/riscv64-unknown-linux-gnu-as)
    set(CMAKE_ASM_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-linux-gnu-as")
    set(CMAKE_C_COMPILER   "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-linux-gnu-gcc")
    set(CMAKE_CXX_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-linux-gnu-g++")
    set(CMAKE_LINKER       "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-linux-gnu-ld")
  else()
    message(FATAL_ERROR "Cannot find tools in the given RISCV_TOOLCHAIN")
  endif()
endif()

set(BASE_DIR ${PROJECT_SOURCE_DIR})

# Linker setup
set(LINKER_SCRIPT ${BASE_DIR}/common/main.ld)
set(CMAKE_ASM_LINKER_FLAGS "-T ${LINKER_SCRIPT} -e main")
set(CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} ${CMAKE_ASM_LINKER_FLAGS} <OBJECTS> -o <TARGET>")

# Enable extensions
add_compile_options(-march=rv64gcv_zba_zbb_zbc_zbs)

include_directories(common)

add_subdirectory(src/nop)
add_subdirectory(src/uart)
add_subdirectory(src/ecall)
add_subdirectory(src/b_ext)
#add_subdirectory(src/dhry)
