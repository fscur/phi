cmake_minimum_required(VERSION 3.0.2)

project(loader)

add_definitions(-DLOADER_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/precompiled)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(loader SHARED ${source_files} ${header_files})

target_link_libraries(loader rendering)

#sdl
find_package(SDL REQUIRED)
if(NOT SDL_FOUND)
    message(SDL not found!)
endif(NOT SDL_FOUND)
target_link_libraries(loader ${SDL_LIBRARY})

#sdl_image
find_package(SDL_IMAGE REQUIRED)
if(NOT SDL_IMAGE_FOUND)
    message(SDL_IMAGE not found!)
endif(NOT SDL_IMAGE_FOUND)
target_link_libraries(loader ${SDL_IMAGE_LIBRARY})

install(TARGETS loader 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pch")

add_custom_command(TARGET loader
    PRE_BUILD
    COMMAND xcopy /D /Y \"$(SolutionDir)src\\precompiled\\precompiled.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")