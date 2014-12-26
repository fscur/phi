cmake_minimum_required(VERSION 3.0.2)

#project settings

project(rendering)

add_definitions(-DRENDERING_EXPORTS)

#includes

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/GLM)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core/headers)
include_directories(headers)

#libs
set (LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${os})
link_directories(${LIBS_DIR})

#packages

find_package(SDL REQUIRED)
find_package(SDL_TTF REQUIRED)
find_package(SDL_IMAGE REQUIRED)
find_package(OPENGL REQUIRED)
#find_package(ASSIMP REQUIRED)

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

#if(NOT ASSIMP_FOUND)
    #message(ASSIMP not found!)
#endif(NOT ASSIMP_FOUND)

#include_directories(${ASSIMP_INCLUDE_DIRS})

include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL_TTF_INCLUDE_DIRS})
include_directories(${SDL_IMAGE_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

#link_directories(${ASSIMP_LIBRARIES})
link_directories(${SDL_LIBRARY})
link_directories(${SDL_TTF_LIBRARIES})
link_directories(${SDL_IMAGE_LIBRARIES})
link_directories(${OPENGL_LIBRARIES})

#GLEW (WIN32)

if (WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIR})
    link_directories(${GLEW_LIBRARY})

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
endif(WIN32)

file(GLOB header_files headers/*.h)
file(GLOB source_files source/*.cpp)

add_library(rendering SHARED ${source_files} ${header_files})

add_dependencies(rendering core)

#target_link_libraries(rendering ${ASSIMP_LIBRARIES})
target_link_libraries(rendering ${SDL_LIBRARY})
target_link_libraries(rendering ${SDL_TTF_LIBRARY})
target_link_libraries(rendering ${SDL_IMAGE_LIBRARY})
target_link_libraries(rendering ${OPENGL_LIBRARY})

if (WIN32)
    target_link_libraries(rendering ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(rendering core)

install(TARGETS rendering 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os} 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    ARCHIVE 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
)