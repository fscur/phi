cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Ddomain_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)
file(GLOB repositories_header_files ./repositories/*.h)

add_library(domain SHARED 
    ${source_files} 
    ${header_files} 
    ${repositories_header_files}
    ${natvis_files})

source_group("Header Files\\Repositories" FILES ${repositories_header_files})
target_link_libraries(domain io)
target_link_libraries(domain context)
target_link_libraries(domain common)
target_link_libraries(domain rendering)

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS domain RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET domain
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET domain
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET domain
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET domain
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) domain)
        
endif(MSVC)

set_target_properties(domain PROPERTIES FOLDER Application)