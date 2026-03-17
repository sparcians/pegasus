project(Pegasus)

file(GLOB FILES LIST_DIRECTORIES true ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/*")

# Each subdirectory is a supported extension
set(EXTS "")
foreach(FILE ${FILES})
  if(IS_DIRECTORY ${FILE})
    cmake_path(GET FILE FILENAME EXT)
    list(APPEND EXTS ${EXT})
  endif()
endforeach()
list(REMOVE_ITEM EXTS "inst_handlers")

set(LIBS "")
foreach(EXT ${EXTS})
  list(APPEND LIBS rv${EXT})
  add_subdirectory(${EXT})
endforeach()

add_library(pegasusinsts)
target_link_libraries(pegasusinsts ${LIBS})

add_dependencies(pegasusinsts AutogenArchFiles)

install(FILES ${PROJECT_BINARY_DIR}/libpegasusinsts.a DESTINATION lib)
