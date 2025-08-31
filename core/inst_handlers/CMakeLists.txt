project(Pegasus)

add_subdirectory(i)
add_subdirectory(m)
add_subdirectory(a)
add_subdirectory(b)
add_subdirectory(f)
add_subdirectory(d)
add_subdirectory(v)
add_subdirectory(zicsr)
add_subdirectory(zifencei)
add_subdirectory(zicond)
add_subdirectory(zcmp)
add_subdirectory(zabha)

add_library(pegasusinsts)
target_link_libraries(pegasusinsts
    rvi
    rvm
    rva
    rvf
    rvd
    rvb
    rvzicsr
    rvzifencei
    rvzicond
    rvzcmp
    rvzabha
    rvv
)
add_dependencies(pegasusinsts AutogenArchFiles)
