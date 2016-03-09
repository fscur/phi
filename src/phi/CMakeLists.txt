cmake_minimum_required(VERSION 3.0.2)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})

if(MSVC)
    file(GLOB header_files *.h)
    file(GLOB source_files *.cpp)
    add_library(phi STATIC ${source_files} ${header_files})
else()
    file(GLOB header_files phi.h)
    file(GLOB source_files *.cpp)
    add_library(phi STATIC ${source_files} ${header_files})
endif(MSVC)

find_package(FREETYPE REQUIRED)
message(${FREETYPE_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ycprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(IntDir)precompiled.pch")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Fd$(IntDir)precompiled.pdb")
endif(MSVC)