#ifndef _PHI_RENDERING_SYSTEM_H_
#define _PHI_RENDERING_SYSTEM_H_

#include <phi/core/resourcesRepository.h>
#include <phi/core/size.h>

#include "rendering.h"

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

namespace phi
{
    static const std::string RENDERING_TEXTURES_PATH = "./textures/";
    static const std::string RENDERING_MESHES_PATH = "./models/";

    struct renderingSystemInfo
    {
    public:
        std::string applicationPath;
        std::string resourcesPath;
        sizef size;
    };

    class renderingSystem
    {

    public:
        RENDERING_API static renderingSystemInfo info;
        RENDERING_API static FT_Library freeTypeLibrary;
        RENDERING_API static bool initialized;

    public:
        RENDERING_API static void init(renderingSystemInfo info);
        RENDERING_API static void resize(sizef viewportSize);
        RENDERING_API static void release();
    };
}

#endif