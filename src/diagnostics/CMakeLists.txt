cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Ddiagnostics_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

if(WIN32)
file(GLOB source_files_platform ./win64/*.cpp)
set(source_files ${source_files} ${source_files_platform})
endif(WIN32)

add_library(diagnostics SHARED ${source_files} ${header_files})

target_link_libraries(diagnostics phi)

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS diagnostics RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET diagnostics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")

    add_custom_command(
        TARGET diagnostics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
                
    add_custom_command(
        TARGET diagnostics
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) diagnostics)
        
endif(MSVC)

set_target_properties(diagnostics PROPERTIES FOLDER Engine)