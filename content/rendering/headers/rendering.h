#ifndef _PHI_RENDERING_H_
#define _PHI_RENDERING_H_

#ifdef WIN32
    #ifdef RENDERING_EXPORTS
        #define RENDERING_API __declspec(dllexport)
    #else
        #define RENDERING_API __declspec(dllimport)
    #endif
#else
    #define RENDERING_API
#endif

#define GLM_FORCE_RADIANS

#include "resourcesRepository.h"

namespace phi
{
    static const std::string RENDERING_TEXTURES_PATH = "./resources/textures/";
    static const std::string RENDERING_MESHES_PATH = "./resources/models/";

    class rendering
    {
    private:
        static void initTextures(std::string path);
        static void initMaterials(std::string path);
        static void initMeshes(std::string path);

    public:
        RENDERING_API static resourcesRepository* repository;
        RENDERING_API static bool initialized;

    public:
        RENDERING_API static void init(std::string path);
        RENDERING_API static void release();
    };
}

#endif