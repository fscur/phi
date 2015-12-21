cmake_minimum_required(VERSION 3.0.2)

#project settings

project(demo)

#includes

include_directories(${INCLUDE_DIRS})

#libs
link_directories(${LIBS_DIRS})

#packages

find_package(SDL REQUIRED)
find_package(SDL_TTF REQUIRED)
find_package(OPENGL REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(BULLET REQUIRED)

if(NOT SDL_FOUND)
    message(SDL not found!)
endif(NOT SDL_FOUND)

if(NOT SDL_TTF_FOUND)
    message(SDLTTF not found!)
endif(NOT SDL_TTF_FOUND)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${BULLET_INCLUDE_DIRS})

if (WIN32)
    find_package(GLEW REQUIRED)

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
endif(WIN32)

file(GLOB header_files ../../inc/phi/demo/*.h)
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

#add_subdirectory(Resources)
source_group("resources\\fonts" FILES ${font_files})
source_group("resources\\models" FILES ${model_files})
source_group("resources\\shaders" FILES ${shader_files})
source_group("resources\\texture" FILES ${texture_files})
source_group("resources\\materials" FILES ${materials_files})
source_group("resources\\cursors" FILES ${cursors_files})
#source_group("Fonts" FILES ${font_files})

add_executable(demo 
    ${source_files} 
    ${header_files} 
    #${font_files} 
    #${model_files} 
    #${shader_files} 
    #${texture_files} 
    #${materials_files} 
    #${cursors_files}
    )

#add_dependencies(demo ui)
add_dependencies(demo scenes)

target_link_libraries(demo ${ASSIMP_LIBRARY})
target_link_libraries(demo ${SDLMAIN_LIBRARY})
target_link_libraries(demo ${SDL_LIBRARY})
target_link_libraries(demo ${SDL_TTF_LIBRARY})
target_link_libraries(demo ${OPENGL_LIBRARY})
target_link_libraries(demo ${FREETYPE_LIBRARY})
target_link_libraries(demo ${BULLET_LIBRARY})
target_link_libraries(demo ${BULLET_COLLISION_LIBRARY})
target_link_libraries(demo ${BULLET_DYNAMICS_LIBRARY})
target_link_libraries(demo ${BULLET_MATH_LIBRARY})

if (WIN32)
    target_link_libraries(demo ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(demo diagnostics)
target_link_libraries(demo core)
target_link_libraries(demo rendering)
#target_link_libraries(demo ui)
target_link_libraries(demo scenes)
target_link_libraries(demo io)
target_link_libraries(demo loader)

install(TARGETS demo 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
)

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