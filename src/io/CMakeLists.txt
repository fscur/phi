cmake_minimum_required(VERSION 3.0.2)
if (MSVC)
    add_definitions(-Dio_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(io SHARED ${source_files} ${header_files})

target_link_libraries(io diagnostics)

install(TARGETS io 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Fd$(IntDir)precompiled.pdb")
    add_custom_command(
        TARGET io
        PRE_BUILD
        COMMAND ../../../../xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
endif(MSVC)

set_target_properties(io PROPERTIES FOLDER Engine)