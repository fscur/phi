#ifndef _PHI_UI_
#define _PHI_UI_

#ifdef WIN32
#ifdef UI_EXPORTS
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#else
#define UI_API
#endif

#define GLM_FORCE_RADIANS

#include "resourcesRepository.h"

namespace phi
{
    static const std::string UI_TEXTURES_PATH = "./resources/textures/";
    static const std::string UI_FONTS_PATH = "./resources/fonts/";

    class ui
    {
    public:
        UI_API static resourcesRepository* repository;

    public:
        UI_API static void init(std::string path);
    };
}

#endif