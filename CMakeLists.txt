cmake_minimum_required(VERSION 3.28)

project(Pegasus C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED ENV{CC})
    find_program(CMAKE_C_COMPILER "clang")
endif()

if(NOT DEFINED ENV{CXX})
    find_program(CMAKE_CXX_COMPILER "clang++")
endif()

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

set (SIM_BASE ${PROJECT_SOURCE_DIR})

# Profile build flags
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG     "-U_FORTIFY_SOURCE -UNDEBUG -O0 -g")

if(DEFINED SPARTA_SEARCH_DIR AND "${SPARTA_SEARCH_DIR}" MATCHES "~")
  message(FATAL_ERROR "Error: SPARTA_SEARCH_DIR should not contain a tilde (~).")
endif()

# Pegasus header-only library
add_library(pegasuslibs INTERFACE)

target_compile_options (pegasuslibs INTERFACE -Werror
  -Wall -Wextra -Winline -Winit-self -Wunused-function
  -Wuninitialized -Wsequence-point -Wno-inline -Wunknown-pragmas
  -Woverloaded-virtual -Wunused-parameter -Wmissing-field-initializers -pipe)

# Use lld for linking clang and gcc builds
if (NOT APPLE)
  target_link_options(pegasuslibs INTERFACE -fuse-ld=lld)
endif ()

# Use ccache if installed
find_program (CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
  set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  message ("-- Using ccache")
endif ()

################################################################################
# Set up Sparta

if(EXISTS ${SPARTA_SEARCH_DIR}/CMakeLists.txt)
  set(BUILD_SPARTA_FROM_SOURCE TRUE)
else()
  set(BUILD_SPARTA_FROM_SOURCE FALSE)
endif()

set(SPARTA_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}")

# To build/use a side sparta, the suggestion is to install sparta in a
# local directory and set SPARTA_SEARCH_DIR to that:
#
#   cd /my/path/to/map/sparta/build
#   cmake --install . --prefix .
#
#   cd pegasus/build
#   SPARTA_SEARCH_DIR=/my/path/to/map/sparta/build cmake ....args
#
if(BUILD_SPARTA_FROM_SOURCE)
  message (STATUS "Building SPARTA from source in ${SPARTA_SEARCH_DIR}")
  add_subdirectory(${SPARTA_SEARCH_DIR} "${CMAKE_BINARY_DIR}/sparta" EXCLUDE_FROM_ALL)
  set_property(TARGET sparta PROPERTY EXCLUDE_FROM_ALL FALSE)
  set_property(TARGET simdb PROPERTY EXCLUDE_FROM_ALL FALSE)
  target_include_directories(pegasuslibs SYSTEM INTERFACE ${SPARTA_SEARCH_DIR})
  target_include_directories(pegasuslibs SYSTEM INTERFACE ${SPARTA_SEARCH_DIR}/simdb/include)
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
  set(SPARTA_LIBS "SPARTA::sparta")

  if (NOT SPARTA_FOUND)
    message (FATAL_ERROR "Sparta was not found in ${SPARTA_SEARCH_DIR}")
  else()
    message (STATUS "Sparta was found in ${SPARTA_SEARCH_DIR}")
  endif()

  target_include_directories(pegasuslibs SYSTEM INTERFACE ${SPARTA_INCLUDE_DIRS})
endif()

################################################################################

target_include_directories(pegasuslibs INTERFACE ${SIM_BASE})
target_include_directories(pegasuslibs INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

# Boost
find_package (Boost REQUIRED COMPONENTS json)
target_include_directories(pegasuslibs INTERFACE ${Boost_INCLUDE_DIRS})

# Mavis
set(MAVIS_CXX_STANDARD 23)
add_subdirectory(mavis SYSTEM)
get_target_property(MAVIS_INCLUDES mavis INCLUDE_DIRECTORIES)
include_directories(${MAVIS_INCLUDES})

# SoftFloat
target_include_directories(pegasuslibs SYSTEM INTERFACE softfloat)
target_include_directories(pegasuslibs SYSTEM INTERFACE softfloat/source/include)
add_subdirectory(softfloat)

# STF Library
set(DISABLE_STF_DOXYGEN 1)
set(DISABLE_STF_TESTS 1)
target_include_directories(pegasuslibs SYSTEM INTERFACE stf_lib)
include(stf_lib/cmake/stf_linker_setup.cmake)
setup_stf_linker(false)
add_subdirectory(stf_lib)

# Pegasus dependencies
add_subdirectory(arch)
add_subdirectory(core)
add_subdirectory(system)
add_subdirectory(sim)
add_subdirectory(cosim)
add_subdirectory(test)

# For install only
add_subdirectory(scripts)
add_subdirectory(include)

include(FetchContent)

FetchContent_Declare(
  spike
  GIT_REPOSITORY https://github.com/riscv-software-src/riscv-isa-sim.git
  GIT_TAG master
  SOURCE_DIR ${SIM_BASE}/spike
)

FetchContent_MakeAvailable(spike)

################################################################################

find_program(CLANGFORMAT_EXECUTABLE NAMES clang-format-15 clang-format-16 clang-format)

execute_process (
  COMMAND
     bash -c "${CLANGFORMAT_EXECUTABLE} --version"
  OUTPUT_VARIABLE
     CLANGFORMAT_VERSION_OUTPUT
)

set(REQUIRED_CLANGFORMAT_VERSION "15")

# Extract the version number from the output
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLANGFORMAT_VERSION ${CLANGFORMAT_VERSION_OUTPUT})

# Compare the found version with the required version
if (CLANGFORMAT_VERSION VERSION_LESS REQUIRED_CLANGFORMAT_VERSION)
    message(FATAL_ERROR "clang-format version ${CLANGFORMAT_VERSION} found. Version ${REQUIRED_CLANGFORMAT_VERSION} or later is required.")
else()
    message(STATUS "clang-format version ${CLANGFORMAT_VERSION} found.")
endif()

execute_process (
  COMMAND
     find core system sim arch include cosim test -name "*.[ch]pp"
  OUTPUT_FILE
     ${CMAKE_CURRENT_BINARY_DIR}/clang-format-files.out
  WORKING_DIRECTORY
     ${SIM_BASE}
)

# CPUTopology is pretty clean as it is.  There's an option to NOT
# collapse the array of structures in clang-format v16, but we're not
# there yet
add_custom_target(clangformat
  COMMAND
     ${CLANGFORMAT_EXECUTABLE} -i --files=${CMAKE_CURRENT_BINARY_DIR}/clang-format-files.out
  WORKING_DIRECTORY
     ${SIM_BASE}
  COMMENT
     "Formatting Pegasus with ${CLANGFORMAT_EXECUTABLE} ..."
  )

add_custom_target(clangformat-check
  COMMAND
     ${CLANGFORMAT_EXECUTABLE} --Werror -n --files=${CMAKE_CURRENT_BINARY_DIR}/clang-format-files.out
  WORKING_DIRECTORY
     ${SIM_BASE}
  COMMENT
     "Formatting Pegasus with ${CLANGFORMAT_EXECUTABLE} ..."
  )

################################################################################

# Install
install(FILES ${PROJECT_BINARY_DIR}/softfloat/libsoftfloat.a DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/stf_lib/lib/libstf.a DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/mavis/libmavis.a DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mavis/json DESTINATION include/pegasus/mavis)

# Versioning
set(MAJOR_VERSION "1")
set(MINOR_VERSION "0")
set(MINOR_MINOR_VERSION "0")
configure_file (include/pegasus_version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/gen/pegasus_version.hpp @ONLY)
