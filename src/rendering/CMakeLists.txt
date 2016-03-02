cmake_minimum_required(VERSION 3.0.2)

#project settings
project(rendering)

add_definitions(-DRENDERING_EXPORTS)

#includes
include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/precompiled)

#libs
link_directories(${LIBS_DIRS})

#packages

#find_package(SDL_TTF REQUIRED)




#if(NOT SDL_TTF_FOUND)
#    message(SDLTTF not found!)
#endif(NOT SDL_TTF_FOUND)




file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB shader_files ../../resources/shaders/*)

add_library(rendering SHARED ${source_files} ${header_files} ${shader_files})
source_group(Shaders FILES ${shader_files})

target_link_libraries(rendering core)
target_link_libraries(rendering io)


#target_link_libraries(rendering ${SDL_TTF_LIBRARY})

#freetype package
find_package(FREETYPE REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(rendering ${FREETYPE_LIBRARY})

#opengl
if (WIN32)
    find_package(OPENGL REQUIRED)
    if(NOT OPENGL_FOUND)
        message(OPENGL not found!)
    endif(NOT OPENGL_FOUND)

    target_link_libraries(rendering ${OPENGL_LIBRARY})
    
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIR})
    link_directories(${GLEW_LIBRARY})

    if(NOT GLEW_FOUND)
        message(GLEW not found!)
    endif(NOT GLEW_FOUND)
    
    target_link_libraries(rendering ${GLEW_LIBRARY})
endif(WIN32)

install(TARGETS rendering 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os} 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pch")

add_custom_command(TARGET rendering
    PRE_BUILD
    COMMAND xcopy /D /Y \"$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")