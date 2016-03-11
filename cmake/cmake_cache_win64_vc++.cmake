set(CMAKE_CXX_FLAGS "/DMSVC /W4 /WX /Za /Zc:wchar_t- /GR- /EHsc" CACHE STRING "c++ compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "c++ debug compiler flags" FORCE)
set(CMAKE_C_FLAGS "/DMSVC /WAll /Wx" CACHE STRING "c compiler flags" FORCE)
set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "c debug compiler flags" FORCE)
set(ROOT_DIR "../../")
set(BIN_FOLDER "win64/vc++" CACHE STRING "")
set(SOLUTION_PLATFORM "x64" CACHE PATH "")
set(PHI_INCLUDE_DIRS "${ROOT_DIR}src/" CACHE PATH "")
set(INCLUDE_DIRS "${ROOT_DIR}inc/" CACHE PATH "")
set(LIBS_DIRS "${CMAKE_SOURCE_DIR}/../../libs/x64" CACHE PATH "")
set(FREETYPE_INCLUDE_DIR_freetype2 "${ROOT_DIR}inc/freetype" CACHE PATH "")
set(FREETYPE_INCLUDE_DIR_ft2build "${ROOT_DIR}inc/freetype" CACHE PATH "")
set(FREETYPE_LIBRARY optimized "${ROOT_DIR}libs/x64/freetype" debug "${ROOT_DIR}libs/x64/freetyped" CACHE PATH "")
set(FREEIMAGE_INCLUDE_DIRS "${ROOT_DIR}inc/freeimage" CACHE PATH "")
set(FREEIMAGE_LIBRARIES optimized "${ROOT_DIR}libs/x64/freeimage" debug "${ROOT_DIR}libs/x64/freeimaged" CACHE PATH "")
set(FREEIMAGE_LIBRARY "freeimage.lib" CACHE PATH "")
set(GLEW_INCLUDE_DIR "${ROOT_DIR}inc/gl" CACHE PATH "")
set(GLEW_LIBRARY optimized "glew32" debug "glew32d" CACHE PATH "")
set(BULLET_INCLUDE_DIR "${ROOT_DIR}inc/bullet" CACHE PATH "")
set(BULLET_COLLISION_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletDynamics" debug "${ROOT_DIR}libs/x64/BulletDynamics_Debug" CACHE PATH "")
set(BULLET_DYNAMICS_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletCollision" debug "${ROOT_DIR}libs/x64/BulletCollision_Debug" CACHE PATH "")
set(BULLET_MATH_LIBRARY optimized "${ROOT_DIR}libs/x64/LinearMath" debug "${ROOT_DIR}libs/x64/LinearMath_Debug" CACHE PATH "")
set(BULLET_SOFTBODY_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletSoftBody" debug "${ROOT_DIR}libs/x64/BulletSoftBody_Debug" CACHE PATH "")