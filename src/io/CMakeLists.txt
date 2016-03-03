cmake_minimum_required(VERSION 3.0.2)

#project(io)

add_definitions(-DIO_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(io SHARED ${source_files} ${header_files})

target_link_libraries(io diagnostics)

install(TARGETS io 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")

    add_custom_command(
        TARGET io
        PRE_BUILD
        COMMAND xcopy /D /Y \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
endif(WIN32)

set_target_properties(io PROPERTIES FOLDER Engine)