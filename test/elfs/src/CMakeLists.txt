project (BASE_TESTS)

set (CMAKE_ASM_FLAGS "-march=rv64i_zicsr_zifencei")

add_subdirectory (nop)
add_subdirectory (add)
add_subdirectory (add_bne)
add_subdirectory (sub_bgeu)
add_subdirectory (lui)
add_subdirectory (sw_lw_blt)
add_subdirectory (misalign)
