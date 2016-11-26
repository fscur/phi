cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dapi_EXPORTS")

include_directories(${INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS})
include_directories(${PHI_INCLUDE_DIRS}/phi)

link_directories(${LIBS_DIRS})

file(GLOB header_files *.h)
file(GLOB source_files *.cpp)
file(GLOB natvis_files *.natvis)

add_executable(demon ${source_files} ${header_files} ${natvis_files})

target_link_libraries(demon application)
target_link_libraries(demon context)
target_link_libraries(demon ui)
target_link_libraries(demon animation)
target_link_libraries(demon loader)
target_link_libraries(demon data)
target_link_libraries(demon layers)
target_link_libraries(demon domain)
target_link_libraries(demon persistence)

find_package(FREETYPE REQUIRED)
link_directories(${FREETYPE_LIBRARIES})
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(demon ${FREETYPE_LIBRARY})
target_link_libraries(demon ${LIBICONV_LIBRARY})
target_link_libraries(demon ${LIBINTL_LIBRARY})

if (MINGW)
    set(install_folder ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${BIN_FOLDER})
    install(TARGETS demon RUNTIME DESTINATION ${install_folder})
endif(MINGW)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yuprecompiled.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fp$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pch")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Fd$(IntDir)precompiled.pdb")
    
    add_custom_command(
        TARGET demon
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.pdb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET demon
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.idb\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET demon
        PRE_BUILD
        COMMAND ../../../../scripts/pre_build_vc++/xcopy_n.bat \"$(SolutionDir)src\\phi\\phi.dir\\$(Configuration)\\precompiled.sbr\" \".\\$(IntDir)\")
        
    add_custom_command(
        TARGET demon
        POST_BUILD
        COMMAND ../../../../scripts/post_build_vc++/demon.bat $(Configuration) demon)
        
endif(MSVC)

set_target_properties(demon PROPERTIES FOLDER Application)
