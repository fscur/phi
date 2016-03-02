cmake_minimum_required(VERSION 3.0.2)

project(scenes)

add_definitions(-DSCENES_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/precompiled)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(scenes SHARED ${source_files} ${header_files})

target_link_libraries(scenes rendering)

install(TARGETS scenes
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/ 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pch")

add_custom_command(TARGET scenes
    PRE_BUILD
    COMMAND xcopy /D /Y \"$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")