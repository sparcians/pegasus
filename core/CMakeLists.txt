project(Pegasus)

add_library(pegasuscore
    OBJECT
    ActionGroup.cpp
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

# Tell cmake that stf_lib is a system include directory
# This suppresses warnings from the stf_lib headers
target_include_directories(pegasuscore SYSTEM PUBLIC stf_lib)

add_subdirectory(inst_handlers)
