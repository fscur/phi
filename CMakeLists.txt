cmake_minimum_required(VERSION 3.0.2)

project(phi)

if (APPLE)
    add_definitions(-DAPPLE)
    set(os osx)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
elseif (WIN32)
    add_definitions(-DWIN32)
    set(os win32)
endif (APPLE)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug|Release")
endif()

add_subdirectory(content/core)
add_subdirectory(content/rendering)
add_subdirectory(content/ui)
add_subdirectory(content/demo)