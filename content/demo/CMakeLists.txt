cmake_minimum_required(VERSION 3.0.2)

#project settings

project(demo)

#includes

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/GLM)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../rendering/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ui/headers)
include_directories(headers)

#libs
set (LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${os}/${CMAKE_BUILD_TYPE})
link_directories(${LIBS_DIR})

#packages

find_package(SDL REQUIRED)
find_package(SDL_TTF REQUIRED)
find_package(OPENGL REQUIRED)

if(NOT SDL_FOUND)
    message(SDL not found!)
endif(NOT SDL_FOUND)

if(NOT SDL_TTF_FOUND)
    message(SDLTTF not found!)
endif(NOT SDL_TTF_FOUND)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL_TTF_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

link_directories(${SDL_LIBRARY})
link_directories(${SDL_TTF_LIBRARIES})
link_directories(${OPENGL_LIBRARIES})

#GLEW (WIN32)

if (WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY})

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
endif(WIN32)

file(GLOB header_files headers/*.h)
file(GLOB source_files source/*.cpp)

set (fonts_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/fonts)
set (models_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/models)
set (shaders_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/shaders)
set (textures_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/textures)

file(GLOB font_files ${fonts_dir}/*.*)
file(GLOB model_files ${models_dir}/*.*)
file(GLOB shader_files ${shaders_dir}/*.*)
file(GLOB texture_files ${textures_dir}/*.*)

#add_subdirectory(Resources)
source_group("Resources\\Fonts" FILES ${font_files})
source_group("Resources\\Models" FILES ${model_files})
source_group("Resources\\Shaders" FILES ${shader_files})
source_group("Resources\\Texture" FILES ${texture_files})
#source_group("Fonts" FILES ${font_files})

add_executable(demo ${source_files} ${header_files} ${font_files} ${model_files} ${shader_files} ${texture_files})

add_dependencies(demo ui)

target_link_libraries(demo ${SDL_LIBRARY})
target_link_libraries(demo ${SDL_TTF_LIBRARY})
target_link_libraries(demo ${OPENGL_LIBRARY})

if (WIN32)
    target_link_libraries(demo ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(demo core)
target_link_libraries(demo rendering)
target_link_libraries(demo ui)

install(TARGETS demo 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE} 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    ARCHIVE 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
)

set (dlls_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${os})
set (fonts_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/fonts)
set (models_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/models)
set (shaders_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/shaders)
set (textures_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/textures)

if (WIN32)
    file(GLOB dll_files ${dlls_dir}/*.dll)
elseif(APPLE)
    file(GLOB dll_files ${dlls_dir}/*.*)
endif()

file(GLOB font_files ${fonts_dir}/*.*)
file(GLOB model_files ${models_dir}/*.*)
file(GLOB shader_files ${shaders_dir}/*.*)
file(GLOB texture_files ${textures_dir}/*.*)

install(FILES ${dll_files}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE})

install(FILES ${font_files}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}/resources/fonts)

install(FILES ${model_files}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}/resources/models)

install(FILES ${shader_files}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}/resources/shaders)

install(FILES ${texture_files}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/${CMAKE_BUILD_TYPE}/resources/textures)