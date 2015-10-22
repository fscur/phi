cmake_minimum_required(VERSION 3.0.2)

project(loader)

add_definitions(-DLOADER_EXPORTS)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc)
include_directories(headers)

file(GLOB header_files headers/*.h)
file(GLOB source_files source/*.cpp)

add_library(loader SHARED ${source_files} ${header_files})

install(TARGETS loader 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    ARCHIVE 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
)