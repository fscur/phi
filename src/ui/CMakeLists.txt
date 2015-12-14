cmake_minimum_required(VERSION 3.0.2)

#project settings

project(ui)

add_definitions(-DUI_EXPORTS)

#includes
include_directories(${INCLUDE_DIRS})

#libs
link_directories(${LIBS_DIRS})

#packages

find_package(OPENGL REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(BULLET REQUIRED)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${BULLET_INCLUDE_DIRS})

link_directories(${OPENGL_LIBRARIES})
link_directories(${FREETYPE_LIBRARIES})

if (WIN32)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY})

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
endif(WIN32)

file(GLOB header_files ../../inc/phi/ui/*.h)
file(GLOB source_files *.cpp)


add_library(ui SHARED ${source_files} ${header_files})

add_dependencies(ui rendering)

target_link_libraries(ui ${OPENGL_LIBRARY})
target_link_libraries(ui ${FREETYPE_LIBRARY})
target_link_libraries(ui ${BULLET_LIBRARY})
target_link_libraries(ui ${BULLET_COLLISION_LIBRARY})
target_link_libraries(ui ${BULLET_DYNAMICS_LIBRARY})
target_link_libraries(ui ${BULLET_MATH_LIBRARY})

if (WIN32)
    target_link_libraries(ui ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(ui diagnostics)
target_link_libraries(ui core)
target_link_libraries(ui rendering)
target_link_libraries(ui scenes)

install(TARGETS ui 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/ 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})