cmake_minimum_required(VERSION 3.0.2)
if (MSVC)
    add_definitions(-DdebugRendering_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
link_directories(${GLEW_LIBRARY})

if(NOT GLEW_FOUND)
    message(GLEW not found!)
endif(NOT GLEW_FOUND)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)

add_library(debugRendering SHARED ${source_files} ${header_files} ${shader_files} ${natvis_files})
target_link_libraries(debugRendering rendering)

#opengl
if (WIN32)
    find_package(OPENGL REQUIRED)
    if(NOT OPENGL_FOUND)
        message(OPENGL not found!)
    endif(NOT OPENGL_FOUND)
    target_link_libraries(debugRendering ${OPENGL_LIBRARY})
endif(WIN32)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(debugRendering ${GLEW_LIBRARY})

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS debugRendering RUNTIME DESTINATION ${install_folder})
endif(MINGW)
    
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET debugRendering
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET debugRendering
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET debugRendering
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET debugRendering
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) debugRendering)
        
endif(MSVC)

set_target_properties(debugRendering PROPERTIES FOLDER Engine)