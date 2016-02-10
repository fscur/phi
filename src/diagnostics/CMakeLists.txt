cmake_minimum_required(VERSION 3.0.2)

project(diagnostics)

add_definitions(-DDIAGNOSTICS_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(diagnostics SHARED ${source_files} ${header_files})

install(TARGETS diagnostics 
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})