cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Dapplication_EXPORTS)
endif(MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)

if (WIN32)
file(GLOB source_files_platform ./win64/*.cpp)
set(source_files ${source_files} ${source_files_platform})
endif(WIN32)

add_library(application SHARED ${source_files} ${header_files} ${natvis_files})

target_link_libraries(application core)
target_link_libraries(application io)

find_package(OPENGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(application ${GLEW_LIBRARY})
target_link_libraries(application ${OPENGL_LIBRARY})

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS application RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET application
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET application
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET application
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET application
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) application)
        
endif(MSVC)

set_target_properties(application PROPERTIES FOLDER Engine)