#ifndef _PHI_RENDERING_SYSTEM_H_
#define _PHI_RENDERING_SYSTEM_H_

#include "rendering.h"
#include "resourcesRepository.h"
#include "defaultRenderTarget.h"
#include "size.h"
#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include <ft2build.h>
#include FT_FREETYPE_H

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
        RENDERING_API static defaultRenderTarget* mainRenderTarget;
        RENDERING_API static resourcesRepository* repository;
        RENDERING_API static FT_Library freeTypeLibrary;
        RENDERING_API static bool initialized;

    public:
        RENDERING_API static void init(renderingSystemInfo info);
        RENDERING_API static void resize(size<GLuint> viewportSize);
        RENDERING_API static void release();
    };
}

#endif