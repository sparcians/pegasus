project(dhrystone_test)

set(CMAKE_C_INCLUDES "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DTIME -DNOENUM -Wno-implicit -save-temps")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-printf -fno-common -falign-functions=4")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xlinker --defsym=__stack_size=0x800 -Xlinker --defsym=__heap_size=0x1000")
add_executable(dhrystone.elf dhry_1.c dhry_2.c strcmp.S)
