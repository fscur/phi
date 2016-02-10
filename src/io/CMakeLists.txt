cmake_minimum_required(VERSION 3.0.2)

project(io)

add_definitions(-DIO_EXPORTS)

include_directories(${INCLUDE_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(io SHARED ${source_files} ${header_files})

target_link_libraries(io diagnostics)

install(TARGETS io 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})