set(ROOT_DIR "../../")
set(SOLUTION_PLATFORM "x64" CACHE PATH "")
set(PHI_INCLUDE_DIRS "${ROOT_DIR}src/" CACHE PATH "")
set(INCLUDE_DIRS "${ROOT_DIR}inc/" CACHE PATH "")
set(LIBS_DIRS "${CMAKE_SOURCE_DIR}/../../libs/mingw" CACHE PATH "")
set(SDL_INCLUDE_DIR "${ROOT_DIR}inc/sdl" CACHE PATH "")
set(SDLMAIN_LIBRARY optimized "${ROOT_DIR}libs/x64/SDL2main" debug "${ROOT_DIR}libs/x64/SDL2maind" CACHE PATH "")
set(SDL_LIBRARY optimized "${ROOT_DIR}libs/x64/SDL2" debug "${ROOT_DIR}libs/x64/SDL2d" CACHE PATH "")
set(SDL_IMAGE_LIBRARY optimized "${ROOT_DIR}libs/x64/SDL2_image" debug "${ROOT_DIR}libs/x64/SDL2_imaged" CACHE PATH "")
set(SDL_IMAGE_INCLUDE_DIR "${ROOT_DIR}inc/sdl" CACHE PATH "")
set(SDL_TTF_LIBRARY optimized "${ROOT_DIR}libs/x64/SDL2_ttf" debug "${ROOT_DIR}libs/x64/SDL2_ttfd" CACHE PATH "")
set(SDL_TTF_INCLUDE_DIR "${ROOT_DIR}inc/sdl" CACHE PATH "")
set(FREETYPE_INCLUDE_DIR_freetype2 "${ROOT_DIR}inc/freetype" CACHE PATH "")
set(FREETYPE_INCLUDE_DIR_ft2build "${ROOT_DIR}inc/freetype" CACHE PATH "")
set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/../../libs/mingw/libfreetype.a" CACHE PATH "")
set(FREEIMAGE_INCLUDE_DIRS "${ROOT_DIR}inc/freeimage" CACHE PATH "")
set(FREEIMAGE_LIBRARIES "${ROOT_DIR}libs/mingw/freeimage" CACHE PATH "")
set(FREEIMAGE_LIBRARY "freeimage.lib" CACHE PATH "")
set(GLEW_INCLUDE_DIR "${ROOT_DIR}inc/gl" CACHE PATH "")
set(GLEW_LIBRARY "glew32" CACHE PATH "")
set(BULLET_INCLUDE_DIR "${ROOT_DIR}inc/bullet" CACHE PATH "")
set(BULLET_COLLISION_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletDynamics" debug "${ROOT_DIR}libs/x64/BulletDynamics_Debug" CACHE PATH "")
set(BULLET_DYNAMICS_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletCollision" debug "${ROOT_DIR}libs/x64/BulletCollision_Debug" CACHE PATH "")
set(BULLET_MATH_LIBRARY optimized "${ROOT_DIR}libs/x64/LinearMath" debug "${ROOT_DIR}libs/x64/LinearMath_Debug" CACHE PATH "")
set(BULLET_SOFTBODY_LIBRARY optimized "${ROOT_DIR}libs/x64/BulletSoftBody" debug "${ROOT_DIR}libs/x64/BulletSoftBody_Debug" CACHE PATH "")