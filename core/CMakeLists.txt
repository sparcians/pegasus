project(Pegasus)

add_library(pegasuscore
    OBJECT
    ActionGroup.cpp
    PegasusCore.cpp
    InstHandlers.cpp
    PegasusState.cpp
    ActionTags.cpp
    Fetch.cpp
    Execute.cpp
    Exception.cpp
    PegasusExtractor.cpp
    PegasusInst.cpp
    VecConfig.cpp
    translate/Translate.cpp
    observers/Observer.cpp
    observers/InstructionLogger.cpp
    observers/SimController.cpp
    observers/STFLogger.cpp
    observers/STFValidator.cpp
    observers/CoSimObserver.cpp
    ../arch/RegisterDefnsJSON.cpp
)
target_link_libraries(pegasuscore PUBLIC pegasuslibs)
add_dependencies(pegasuscore AutogenArchFiles)

add_subdirectory(inst_handlers)
