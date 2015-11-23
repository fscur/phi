#ifndef _PHI_RENDERING_SYSTEM_H_
#define _PHI_RENDERING_SYSTEM_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include <freetype/ft2build.h>
#include <freetype\freetype.h>
//#include FT_FREETYPE_H

#include "phi/rendering/rendering.h"
#include "phi/core/resourcesRepository.h"
#include "phi/rendering/defaultFrameBuffer.h"
#include "phi/rendering/pickingFrameBuffer.h"
#include "phi/core/size.h"

namespace phi
{
    static const std::string RENDERING_TEXTURES_PATH = "./resources/textures/";
    static const std::string RENDERING_MESHES_PATH = "./resources/models/";

    struct renderingSystemInfo
    {
    public:
        std::string applicationPath;
        size<GLuint> size;
    };

    class renderingSystem
    {

    public:
        RENDERING_API static renderingSystemInfo info;
        RENDERING_API static defaultFrameBuffer* defaultFrameBuffer;
		RENDERING_API static pickingFrameBuffer* pickingFrameBuffer;
        RENDERING_API static FT_Library freeTypeLibrary;
        RENDERING_API static bool initialized;

    public:
        RENDERING_API static void init(renderingSystemInfo info);
        RENDERING_API static void resize(size<GLuint> viewportSize);
        RENDERING_API static void release();
    };
}

#endif