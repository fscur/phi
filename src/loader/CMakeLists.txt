cmake_minimum_required(VERSION 3.0.2)

if (MSVC)
    add_definitions(-DLOADER_EXPORTS)
endif (MSVC)

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})
find_package(FREEIMAGE REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_IMAGE REQUIRED)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(loader SHARED ${source_files} ${header_files})

target_link_libraries(loader rendering)
target_link_libraries(loader ${SDL_LIBRARY})
target_link_libraries(loader ${SDL_IMAGE_LIBRARY})
target_link_libraries(loader ${FREEIMAGE_LIBRARY})

install(TARGETS loader 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")

    add_custom_command(
        TARGET loader
        PRE_BUILD
        COMMAND xcopy /D /Y \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
endif(MSVC)

set_target_properties(loader PROPERTIES FOLDER Engine)