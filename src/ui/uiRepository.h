#pragma once
#include <phi.h>
#include "uiApi.h"

#include <core\resourcesRepository.h>
#include <core\size.h>
#include <rendering\fontManager.h>
#include <rendering\font.h>

#include "cursor.h"

namespace phi
{
    static const string UI_FONTS_PATH = "fonts";
    static const string UI_IMAGES_PATH = "images";
    static const string UI_CURSORS_PATH = "cursors";

    struct uiRepositoryInfo
    {
    public:
        fontManager* fontManager;
        string applicationPath;
        string resourcesPath;
        sizef size;
    };

    class uiRepository
    {
    private:
        static void loadTextures(string directory);
        static void loadCursors(string directory);
        static void loadFonts(string directory, fontManager* fontManager);
    public:
        static font* fontConsolas14;

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