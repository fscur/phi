cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Dphysics_EXPORTS)
endif(MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)

file(GLOB sweep_source_files ./sweep/*.cpp)
file(GLOB sweep_header_files ./sweep/*.h)
file(GLOB intersection_source_files ./intersection/*.cpp)
file(GLOB intersection_header_files ./intersection/*.h)

add_library(physics SHARED
    ${source_files}
    ${header_files}
    ${natvis_files}
    ${sweep_header_files}
    ${sweep_source_files}
    ${intersection_header_files}
    ${intersection_source_files})

source_group("Source Files\\sweep" FILES ${sweep_source_files})
source_group("Header Files\\sweep" FILES ${sweep_header_files})
source_group("Source Files\\intersection" FILES ${intersection_source_files})
source_group("Header Files\\intersection" FILES ${intersection_header_files})

target_link_libraries(physics core)
target_link_libraries(physics ${PHYSX_LIBRARY})
target_link_libraries(physics ${PHYSX_COMMON_LIBRARY})
target_link_libraries(physics ${PHYSX_EXTENSIONS_LIBRARY})
target_link_libraries(physics ${PHYSX_PROFILESDK_LIBRARY})

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS physics RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    add_custom_command(
        TARGET physics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
    
    add_custom_command(
        TARGET physics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET physics
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET physics
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) physics)
endif(MSVC)

set_target_properties(physics PROPERTIES FOLDER Engine)