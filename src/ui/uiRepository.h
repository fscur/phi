#pragma once
#include <phi.h>

#include "ui.h"

#include <core\resourcesRepository.h>
#include <core\size.h>

#include <rendering\font.h>

#include "cursor.h"

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
        sizeui size;
    };

    class uiRepository
    {
    private:
        static void loadTextures(std::string directory);
        static void loadCursors(std::string directory);
        static void loadFonts(std::string directory);
    public:
        static font* fontConsolas14;
        static font* fontConsolas16;
        static font* fontConsolas18;
        static font* fontConsolas24;
        static font* fontConsolas100;
        static font* fontArial72;

        static cursor* cursorDefault;
        static cursor* cursorText;

        static texture* textureButton;
        static texture* textureArrowDown;
        static texture* textureArrowUp;
        static texture* textureModels;
        static texture* textureMaterials;

        static bool initialized;

    public:
        static void init(uiRepositoryInfo info);
    };
}