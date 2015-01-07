#ifndef _PHI_UI_SYSTEM_H_
#define _PHI_UI_SYSTEM_H_

#include "ui.h"
#include "resourcesRepository.h"
#include "size.h"
#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    static const std::string UI_TEXTURES_PATH = "./resources/textures/";
    static const std::string UI_FONTS_PATH = "./resources/fonts/";

    struct uiSystemInfo
    {
    public:
        std::string applicationPath;
        size<GLuint> size;
    };

    class uiSystem
    {
    public:
        UI_API static resourcesRepository* repository;

    public:
        UI_API static void init(uiSystemInfo info);
    };
}

#endif