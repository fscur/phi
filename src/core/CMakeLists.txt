cmake_minimum_required(VERSION 3.0.2)

project(core)

add_definitions(-DCORE_EXPORTS)

include_directories(${INCLUDE_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(core SHARED ${source_files} ${header_files})

add_dependencies(core diagnostics)
target_link_libraries(core diagnostics)

install(TARGETS core 
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})