cmake_minimum_required(VERSION 3.0.2)

project(core)

add_definitions(-DCORE_EXPORTS)

include_directories(${INCLUDE_DIRS})

file(GLOB header_files ../../inc/phi/core/*.h)
file(GLOB source_files *.cpp)

add_library(core SHARED ${source_files} ${header_files})

install(TARGETS core 
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})