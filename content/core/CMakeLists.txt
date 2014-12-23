cmake_minimum_required(VERSION 3.0.2)

project(core)

add_definitions(-DCORE_EXPORTS)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/GLM)
include_directories(headers)

file(GLOB header_files headers/*.h)
file(GLOB source_files source/*.cpp)

add_library(core SHARED ${source_files} ${header_files})

install(TARGETS core 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE} 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    ARCHIVE 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
)