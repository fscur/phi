cmake_minimum_required(VERSION 3.0.2)

#project settings

project(io)

add_definitions(-DIO_EXPORTS)

#includes

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core/headers)
include_directories(headers)

#libs
set (LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${os})
link_directories(${LIBS_DIR})

file(GLOB header_files headers/*.h)
file(GLOB source_files source/*.cpp)

add_library(io SHARED ${source_files} ${header_files})

add_dependencies(io core)

target_link_libraries(io core)

install(TARGETS io 
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