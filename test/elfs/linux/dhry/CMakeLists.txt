
project(dhrystone C)

add_executable (dhry.elf dhry.c)

add_definitions(-DTIME)
