cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Ddiagnostics_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(diagnostics SHARED ${source_files} ${header_files})

target_link_libraries(diagnostics phi)

install(TARGETS diagnostics 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET diagnostics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET diagnostics
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project_$(Configuration).bat $(Configuration) diagnostics)
        
endif(MSVC)

set_target_properties(diagnostics PROPERTIES FOLDER Engine)