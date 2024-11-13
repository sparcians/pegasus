cmake_minimum_required(VERSION 3.27.6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

project(Atlas CXX)
set (SIM_BASE ${PROJECT_SOURCE_DIR})

# Profile build flags
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O3 -g")
set(CMAKE_CXX_FLAGS_DEBUG     "-U_FORTIFY_SOURCE -UNDEBUG -O0 -g")

if(EXISTS ${SPARTA_SEARCH_DIR}/CMakeLists.txt)
  set(BUILD_SPARTA_FROM_SOURCE TRUE)
else()
  set(BUILD_SPARTA_FROM_SOURCE FALSE)
endif()

if(NOT BUILD_SPARTA_FROM_SOURCE)
  add_compile_options ( -Werror
    -Wall -Wextra -Winline -Winit-self -Wunused-function
    -Wuninitialized -Wsequence-point -Winline -Wunknown-pragmas
    -Woverloaded-virtual -Wunused-parameter -Wmissing-field-initializers -pipe)
else()
  add_compile_options (
    -Winline -Winit-self -Wunused-function
    -Wuninitialized -Wsequence-point -Winline -Wunknown-pragmas
    -Woverloaded-virtual -Wmissing-field-initializers -pipe)
endif()

################################################################################
# Set up Sparta

set(SPARTA_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}")

if(BUILD_SPARTA_FROM_SOURCE)
  message (STATUS "Building SPARTA from source in ${SPARTA_SEARCH_DIR}")
  add_subdirectory(${SPARTA_SEARCH_DIR} "${CMAKE_BINARY_DIR}/sparta")
  include_directories(${SPARTA_SEARCH_DIR})
  include(${SPARTA_SEARCH_DIR}/cmake/SpartaTestingMacros.cmake)
  include(${SPARTA_SEARCH_DIR}/cmake/sparta-config.cmake)
  set(SPARTA_LIBS ${Sparta_LIBS})
else()
  if(IS_DIRECTORY ${SPARTA_SEARCH_DIR})
    message (STATUS "Using '${SPARTA_SEARCH_DIR}' for sparta install")
  elseif(DEFINED ENV{CONDA_PREFIX})
    message (STATUS "Looking for SPARTA in the conda environment: '$ENV{CONDA_PREFIX}'")
    set(SPARTA_SEARCH_DIR $ENV{CONDA_PREFIX})
  else()
    message (STATUS "If needed, please provide the location where sparta is installed: -DSPARTA_SEARCH_DIR=<directory>")
  endif()
  set(CMAKE_MODULE_PATH "${SPARTA_SEARCH_DIR}/lib/cmake/sparta" ${CMAKE_MODULE_PATH})

  find_package(Sparta REQUIRED)

  if (NOT SPARTA_FOUND)
    message (FATAL_ERROR "Sparta was not found in ${SPARTA_SEARCH_DIR}")
  else()
    message (STATUS "Sparta was found in ${SPARTA_SEARCH_DIR}")
    set(SPARTA_LIBS SPARTA::sparta)
  endif()

  include_directories(${SPARTA_INCLUDE_DIRS})
endif()

################################################################################

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

include_directories(${CMAKE_SOURCE_DIR})

# Mavis
include_directories(SYSTEM mavis)

# ELFIO
include_directories(SYSTEM elfio)

# 32-bit register JSON directory
add_compile_definitions(REG32_JSON_DIR="${SIM_BASE}/arch/rv32")
add_compile_definitions(REG64_JSON_DIR="${SIM_BASE}/arch/rv64")

add_subdirectory(mavis)
add_subdirectory(arch)
add_subdirectory(core)
add_subdirectory(core/inst_handlers)
add_subdirectory(system)
add_subdirectory(sim)
add_subdirectory(cosim)
add_subdirectory(test)

include(FetchContent)

FetchContent_Declare(
  spike
  GIT_REPOSITORY https://github.com/riscv-software-src/riscv-isa-sim.git
  GIT_TAG master
  SOURCE_DIR ${SIM_BASE}/spike
)

FetchContent_MakeAvailable(spike)

################################################################################

if(NOT CLANGFORMAT_EXECUTABLE)
  set(CLANGFORMAT_EXECUTABLE clang-format)
endif()

# CPUTopology is pretty clean as it is.  There's an option to NOT
# collapse the array of structures in clang-format v16, but we're not
# there yet
add_custom_target(clangformat
  find core -name "*.[hc]pp" > clang-format-files.out
  COMMAND
     ${CLANGFORMAT_EXECUTABLE} -i --files=clang-format-files.out
  WORKING_DIRECTORY
     ${CMAKE_SOURCE_DIR}
  COMMENT
     "Formatting ${prefix} with ${CLANGFORMAT_EXECUTABLE} ..."
  )

add_custom_target(clangformat-check
  find core -name "*.[hc]pp" > clang-format-files.out
  COMMAND
     ${CLANGFORMAT_EXECUTABLE} --Werror -n --files=clang-format-files.out
  WORKING_DIRECTORY
     ${CMAKE_SOURCE_DIR}
  COMMENT
     "Formatting ${prefix} with ${CLANGFORMAT_EXECUTABLE} ..."
  )
