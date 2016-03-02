cmake_minimum_required(VERSION 3.0.2)

project(precompiled)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(precompiled STATIC ${source_files} ${header_files})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ycprecompiled.h")