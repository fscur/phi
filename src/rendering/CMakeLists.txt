cmake_minimum_required(VERSION 3.0.2)

#project settings
project(rendering)

add_definitions(-DRENDERING_EXPORTS)

#includes
include_directories(${INCLUDE_DIRS})

#libs
link_directories(${LIBS_DIRS})

#packages
find_package(SDL REQUIRED)
find_package(SDL_TTF REQUIRED)
find_package(SDL_IMAGE REQUIRED)
find_package(OPENGL REQUIRED)
find_package(FREETYPE REQUIRED)

if(NOT SDL_FOUND)
    message(SDL not found!)
endif(NOT SDL_FOUND)

if(NOT SDL_TTF_FOUND)
    message(SDLTTF not found!)
endif(NOT SDL_TTF_FOUND)

if(NOT SDL_IMAGE_FOUND)
    message(SDL_IMAGE not found!)
endif(NOT SDL_IMAGE_FOUND)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

include_directories(${FREETYPE_INCLUDE_DIRS})

if (WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIR})
    link_directories(${GLEW_LIBRARY})

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
endif(WIN32)

file(GLOB header_files ../../inc/phi/rendering/*.h)
file(GLOB source_files *.cpp)

add_library(rendering SHARED ${source_files} ${header_files})

add_dependencies(rendering core)
add_dependencies(rendering io)
add_dependencies(rendering loader)

target_link_libraries(rendering ${SDL_LIBRARY})
target_link_libraries(rendering ${SDL_TTF_LIBRARY})
target_link_libraries(rendering ${SDL_IMAGE_LIBRARY})
target_link_libraries(rendering ${OPENGL_LIBRARY})
target_link_libraries(rendering ${FREETYPE_LIBRARY})

if (WIN32)
    target_link_libraries(rendering ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(rendering diagnostics)
target_link_libraries(rendering core)
target_link_libraries(rendering io)
target_link_libraries(rendering loader)

install(TARGETS rendering 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os} 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})