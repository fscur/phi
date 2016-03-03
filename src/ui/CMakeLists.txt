cmake_minimum_required(VERSION 3.0.2)

#project(ui)

add_definitions(-DUI_EXPORTS)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(ui SHARED ${source_files} ${header_files})
target_link_libraries(ui loader)

install(TARGETS ui 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}/ 
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})
        
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")

    add_custom_command(
        TARGET ui
        PRE_BUILD
        COMMAND xcopy /D /Y \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
endif(WIN32)

set_target_properties(ui PROPERTIES FOLDER Engine)