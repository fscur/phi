cmake_minimum_required(VERSION 3.0.2)

project(precompiled)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})

find_package(FREETYPE REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(precompiled STATIC ${source_files} ${header_files})

find_package(FREETYPE REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ycprecompiled.h")