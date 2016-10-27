cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Ddata_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})
find_package(FREEIMAGE REQUIRED)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)
file(GLOB abstractions_header_files ./abstractions/*.h)
FILE(GLOB model_header_files ./model/*.h)
FILE(GLOB utils_header_files ./utils/*.h)

add_library(data SHARED 
    ${source_files} 
    ${abstractions_header_files} 
    ${model_header_files} 
    ${utils_header_files} 
    ${header_files} 
    ${natvis_files})

source_group("Header Files\\Abstractions" FILES ${abstractions_header_files})
source_group("Header Files\\Model" FILES ${model_header_files})
source_group("Header Files\\Utils" FILES ${utils_header_files})
target_link_libraries(data io)
target_link_libraries(data common)
target_link_libraries(data rendering)
target_link_libraries(data ${FREEIMAGE_LIBRARY})

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS data RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET data
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET data
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET data
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET data
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) data)
        
endif(MSVC)

set_target_properties(data PROPERTIES FOLDER Engine)