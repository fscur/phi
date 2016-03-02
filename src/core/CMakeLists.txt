cmake_minimum_required(VERSION 3.0.2)

project(core)

add_definitions(-DCORE_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/precompiled)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(core SHARED ${source_files} ${header_files})

target_link_libraries(core diagnostics)

install(TARGETS core 
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pch")

add_custom_command(TARGET core
    PRE_BUILD
    COMMAND xcopy /D /Y \"$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")

