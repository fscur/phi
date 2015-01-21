#ifndef _PHI_RENDERING_SYSTEM_H_
#define _PHI_RENDERING_SYSTEM_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include "rendering.h"
#include "resourcesRepository.h"
#include "defaultFrameBuffer.h"
#include "pickingFrameBuffer.h"
#include "size.h"

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
    private:
        static void initTextures();
        static void initMaterials();
        static void initMeshes();

    public:
        RENDERING_API static renderingSystemInfo info;
        RENDERING_API static defaultFrameBuffer* defaultFrameBuffer;
		RENDERING_API static pickingFrameBuffer* pickingFrameBuffer;
        RENDERING_API static resourcesRepository* repository;
        RENDERING_API static bool initialized;

    public:
        RENDERING_API static void init(renderingSystemInfo info);
        RENDERING_API static void resize(size<GLuint> viewportSize);
        RENDERING_API static void release();
    };
}

#endif