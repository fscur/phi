#ifndef _PHI_UI_REPOSITORY_H_
#define _PHI_UI_REPOSITORY_H_

#include "ui.h"

#include <core\resourcesRepository.h>
#include <core\size.h>

#include <rendering\font.h>

#include "cursor.h"

#if WIN32
#include <GL\glew.h>
#else
#include <OpenGL\gl3.h>
#endif

namespace phi
{
    static const std::string UI_FONTS_PATH = "fonts";
    static const std::string UI_IMAGES_PATH = "images";
    static const std::string UI_CURSORS_PATH = "cursors";

    struct uiRepositoryInfo
    {
    public:
        std::string applicationPath;
        std::string resourcesPath;
        sizef size;
    };

    class uiRepository
    {
    private:
        static void loadTextures(std::string directory);
        static void loadCursors(std::string directory);
        static void loadFonts(std::string directory);
    public:
        UI_API static font* fontConsolas14;
        UI_API static font* fontConsolas16;
        UI_API static font* fontConsolas18;
        UI_API static font* fontConsolas24;
        UI_API static font* fontConsolas100;
        UI_API static font* fontArial72;

        UI_API static cursor* cursorDefault;
        UI_API static cursor* cursorText;

        UI_API static texture* textureButton;
        UI_API static texture* textureArrowDown;
        UI_API static texture* textureArrowUp;
        UI_API static texture* textureModels;
        UI_API static texture* textureMaterials;

        UI_API static bool initialized;

    public:
        UI_API static void init(uiRepositoryInfo info);
    };
}

#endif