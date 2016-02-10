cmake_minimum_required(VERSION 3.0.2)

project(loader)

add_definitions(-DLOADER_EXPORTS)

include_directories(${INCLUDE_DIRS})

link_directories(${LIBS_DIRS})

find_package(SDL REQUIRED)
find_package(SDL_TTF REQUIRED)
find_package(SDL_IMAGE REQUIRED)
find_package(OPENGL REQUIRED)
find_package(FREETYPE REQUIRED)

if(NOT SDL_FOUND)
    message(SDL not found!)
endif(NOT SDL_FOUND)

if(NOT SDL_TTF_FOUND)
    message(SDLTTF not found!)
endif(NOT SDL_TTF_FOUND)

if(NOT SDL_IMAGE_FOUND)
    message(SDL_IMAGE not found!)
endif(NOT SDL_IMAGE_FOUND)

if(NOT OPENGL_FOUND)
    message(OPENGL not found!)
endif(NOT OPENGL_FOUND)

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)

add_library(loader SHARED ${source_files} ${header_files})

add_dependencies(loader core)
add_dependencies(loader io)
add_dependencies(loader rendering)

target_link_libraries(loader ${SDL_LIBRARY})
target_link_libraries(loader ${SDL_TTF_LIBRARY})
target_link_libraries(loader ${SDL_IMAGE_LIBRARY})
target_link_libraries(loader ${OPENGL_LIBRARY})
target_link_libraries(loader ${FREETYPE_LIBRARY})

if (WIN32)
    target_link_libraries(loader ${GLEW_LIBRARY})
endif (WIN32)

target_link_libraries(loader core)
target_link_libraries(loader io)
target_link_libraries(loader rendering)
target_link_libraries(loader diagnostics)

install(TARGETS loader 
    RUNTIME 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${os}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})