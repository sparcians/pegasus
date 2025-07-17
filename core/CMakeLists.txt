project(Pegasus)

add_library(pegasuscore
    STATIC
    ActionGroup.cpp
    PegasusState.cpp
    ActionTags.cpp
    Fetch.cpp
    Execute.cpp
    Exception.cpp
    PegasusExtractor.cpp
    PegasusInst.cpp
    translate/Translate.cpp
    observers/Observer.cpp
    observers/CoSimObserver.cpp
    observers/InstructionLogger.cpp
    observers/SimController.cpp
    ../arch/RegisterDefnsJSON.cpp
)

target_include_directories(pegasuscore PUBLIC ${CMAKE_BINARY_DIR})
add_dependencies(pegasuscore AutogenArchFiles)
add_dependencies(pegasuscore pegasusinsts)
