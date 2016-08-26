cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-Dlayers_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)

file(GLOB camera_control_source_files ./cameraControl/*.cpp)
file(GLOB camera_control_header_files ./cameraControl/*.h)
file(GLOB node_creation_source_files ./nodeCreation/*.cpp)
file(GLOB node_creation_header_files ./nodeCreation/*.h)
file(GLOB node_selection_source_files ./nodeSelection/*.cpp)
file(GLOB node_selection_header_files ./nodeSelection/*.h)
file(GLOB node_translation_source_files ./nodeTranslation/*.cpp)
file(GLOB node_translation_header_files ./nodeTranslation/*.h)

add_library(layers SHARED 
    ${source_files}
    ${header_files}
    ${natvis_files}
    ${camera_control_source_files} 
    ${camera_control_header_files}
    ${node_creation_source_files} 
    ${node_creation_header_files}
    ${node_selection_source_files} 
    ${node_selection_header_files}
    ${node_translation_source_files} 
    ${node_translation_header_files})
     
source_group("Source Files\\cameraControl" FILES ${camera_control_source_files})
source_group("Header Files\\cameraControl" FILES ${camera_control_header_files})
source_group("Source Files\\nodeCreation" FILES ${node_creation_source_files})
source_group("Header Files\\nodeCreation" FILES ${node_creation_header_files})
source_group("Source Files\\nodeSelection" FILES ${node_selection_source_files})
source_group("Header Files\\nodeSelection" FILES ${node_selection_header_files})
source_group("Source Files\\nodeTranslation" FILES ${node_translation_source_files})
source_group("Header Files\\nodeTranslation" FILES ${node_translation_header_files})

target_link_libraries(layers context)
target_link_libraries(layers animation)
target_link_libraries(layers physics)
target_link_libraries(layers uiRendering)
target_link_libraries(layers debugRendering)
target_link_libraries(layers sceneRendering)

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS layers RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")

    add_custom_command(
        TARGET layers
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")

    add_custom_command(
        TARGET layers
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET layers
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
                
    add_custom_command(
        TARGET layers
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/project.bat $(Configuration) layers)
        
endif(MSVC)

set_target_properties(layers PROPERTIES FOLDER Engine)