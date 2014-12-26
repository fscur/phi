cmake_minimum_required(VERSION 3.0.2)

#project settings

project(ui)

add_definitions(-DUI_EXPORTS)

#includes

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/GLM)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/SDL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../rendering/headers)
include_directories(headers)

#libs
set (LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${os})
link_directories(${LIBS_DIR})

#packages

#find_package(SDL REQUIRED)
#find_package(SDL_TTF REQUIRED)
find_package(OPENGL REQUIRED)

#if(NOT SDL_FOUND)
    #message(SDL not found!)
#endif(NOT SDL_FOUND)

#if(NOT SDL_TTF_FOUND)
    #message(SDLTTF not found!)
#endif(NOT SDL_TTF_FOUND)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

#include_directories(${SDL_INCLUDE_DIR})
#include_directories(${SDL_TTF_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

#link_directories(${SDL_LIBRARY})
#link_directories(${SDL_TTF_LIBRARIES})
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

add_library(ui SHARED ${source_files} ${header_files})

add_dependencies(ui rendering)

#target_link_libraries(ui ${SDL_LIBRARY})
#target_link_libraries(ui ${SDL_TTF_LIBRARY})
target_link_libraries(ui ${OPENGL_LIBRARY})

if (WIN32)
    target_link_libraries(ui ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(ui core)
target_link_libraries(ui rendering)

install(TARGETS ui 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/ 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    ARCHIVE 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
)