project(Pegasus)

add_library(pegasusinsts)
target_link_libraries(pegasusinsts
    rvi
    rvm
    rva
    rvf
    rvd
    rvb
    rvv
    rvzicsr
    rvzifencei
    rvzihintpause
    rvzicond
    rvzcmp
    rvzcmt
    rvzabha
    rvzilsd
    rvzfa
    rvzfh
    rvv
    rvh
)

add_subdirectory(i)
add_subdirectory(m)
add_subdirectory(a)
add_subdirectory(f)
add_subdirectory(d)
add_subdirectory(b)
add_subdirectory(zicsr)
add_subdirectory(zifencei)
add_subdirectory(zihintpause)
add_subdirectory(zicond)
add_subdirectory(zcmp)
add_subdirectory(zcmt)
add_subdirectory(zabha)
add_subdirectory(zilsd)
add_subdirectory(zfa)
add_subdirectory(zfh)
add_subdirectory(v)
add_subdirectory(h)

add_dependencies(pegasusinsts AutogenArchFiles)

install(FILES ${PROJECT_BINARY_DIR}/libpegasusinsts.a DESTINATION lib)
