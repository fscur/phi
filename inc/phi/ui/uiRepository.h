#ifndef _PHI_UI_REPOSITORY_H_
#define _PHI_UI_REPOSITORY_H_

#include "phi/ui/ui.h"
#include "phi/core/resourcesRepository.h"
#include "phi/core/size.h"
#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

namespace phi
{
    static const std::string UI_TEXTURES_PATH = "./textures/";
    static const std::string UI_FONTS_PATH = "./fonts/";
    static const std::string UI_CURSORS_PATH = "./cursors/";

    struct uiRepositoryInfo
    {
    public:
        std::string applicationPath;
        std::string resourcesPath;
        sizef size;
    };

    class uiRepository
    {
    public:
        UI_API static resourcesRepository* repository;
        UI_API static bool initialized;

    public:
        UI_API static void init(uiRepositoryInfo info);
    };
}

#endif