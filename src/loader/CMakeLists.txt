cmake_minimum_required(VERSION 3.0.2)

project(loader)

add_definitions(-DLOADER_EXPORTS)

include_directories(${INCLUDE_DIRS})

file(GLOB header_files ../../inc/phi/loader/*.h)
file(GLOB source_files *.cpp)

add_library(loader SHARED ${source_files} ${header_files})

target_link_libraries(loader diagnostics)

install(TARGETS loader 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})