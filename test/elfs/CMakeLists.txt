cmake_minimum_required(VERSION 3.27.6)


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR rv64gcv_zba_zbb_zbc_zbs)

# Unset these variables to prevent include of Mac SDK
set(CMAKE_OSX_SYSROOT "/")
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

if(NOT RISCV_TOOLCHAIN)
  message(FATAL_ERROR "You must supply a -DRISCV_TOOLCHAIN=/path")
endif()

set(BASE_DIR ${PROJECT_SOURCE_DIR})

# Linker setup
set(LINKER_SCRIPT ${BASE_DIR}/common/main.ld)
set(CMAKE_ASM_LINKER_FLAGS "-T ${LINKER_SCRIPT} -e main")
set(CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} ${CMAKE_ASM_LINKER_FLAGS} <OBJECTS> -o <TARGET>")

# Enable extensions
add_compile_options(-march=rv64gcv_zba_zbb_zbc_zbs)

include_directories(common)

add_subdirectory(asm)
# add_subdirectory(lilnux)
