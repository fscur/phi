cmake_minimum_required(VERSION 3.0.2)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

set (resources_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources)

set (fonts_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/fonts)
set (models_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/models)
set (shaders_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/shaders)
set (textures_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/textures)
set (materials_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/materials)
set (cursors_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cursors)

file(GLOB font_files ${fonts_dir}/*.*)
file(GLOB model_files ${models_dir}/*.*)
file(GLOB shader_files ${shaders_dir}/*.*)
file(GLOB texture_files ${textures_dir}/*.*)
file(GLOB materials_files ${materials_dir}/*.*)
file(GLOB cursors_files ${cursors_dir}/*.*)

source_group("resources\\fonts" FILES ${font_files})
source_group("resources\\models" FILES ${model_files})
source_group("resources\\shaders" FILES ${shader_files})
source_group("resources\\texture" FILES ${texture_files})
source_group("resources\\materials" FILES ${materials_files})
source_group("resources\\cursors" FILES ${cursors_files})

add_executable(demon ${source_files} ${header_files})

target_link_libraries(demon apps)
target_link_libraries(demon scenes)
target_link_libraries(demon ui)

find_package(FREETYPE REQUIRED)
link_directories(${FREETYPE_LIBRARIES})
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(demon ${FREETYPE_LIBRARY})

install(TARGETS demon 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

set (dlls_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${SOLUTION_PLATFORM})

if (WIN32)
    file(GLOB dll_files ${dlls_dir}/*.dll)
elseif(APPLE)
    file(GLOB dll_files ${dlls_dir}/*.*)
endif()

set (image_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../images)
file(GLOB image_files ${image_dir}/*.*)

install(FILES ${image_files}
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

install(FILES ${dll_files}
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

install(DIRECTORY ${resources_dir}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os})

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")

    add_custom_command(
        TARGET demon
        PRE_BUILD
        COMMAND xcopy /D /Y \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
endif(MSVC)

set_target_properties(demon PROPERTIES FOLDER App)
