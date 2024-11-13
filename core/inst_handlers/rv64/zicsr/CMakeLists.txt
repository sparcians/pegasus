project(Atlas)

add_library(rvzicsr
    OBJECT
    RvzicsrInsts.cpp
    csrrc.cpp
    csrrci.cpp
    csrrs.cpp
    csrrsi.cpp
    csrrw.cpp
    csrrwi.cpp
)
