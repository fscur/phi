#include "uiRepository.h"

#include <io\path.h>

#include <loader\importer.h>

#include <rendering\texture.h>
#include <rendering\font.h>

#include "cursor.h"

namespace phi
{
    font* uiRepository::fontConsolas14;
    font* uiRepository::fontConsolas16;
    font* uiRepository::fontConsolas18;
    font* uiRepository::fontConsolas24;
    font* uiRepository::fontConsolas100;
    font* uiRepository::fontArial72;

    cursor* uiRepository::cursorDefault;
    cursor* uiRepository::cursorText;

    texture* uiRepository::textureButton;
    texture* uiRepository::textureArrowDown;
    texture* uiRepository::textureArrowUp;
    texture* uiRepository::textureModels;
    texture* uiRepository::textureMaterials;

    bool uiRepository::initialized = false;

    void uiRepository::init(uiRepositoryInfo info)
    {
        loadFonts(phi::path::combine(info.resourcesPath, UI_FONTS_PATH));
        loadTextures(phi::path::combine(info.resourcesPath, UI_IMAGES_PATH));
        loadCursors(phi::path::combine(info.resourcesPath, UI_CURSORS_PATH));

        initialized = true;
    }

    void uiRepository::loadTextures(std::string directory)
    {
        phi::resource<phi::texture>* textureButtonRes;
        phi::importer::importTexture(phi::path::combine(directory, "button.texture"), textureButtonRes);
        textureButton = textureButtonRes->getObject();

        phi::resource<phi::texture>* textureArrowDownRes;
        phi::importer::importTexture(phi::path::combine(directory, "arrow_down.texture"), textureArrowDownRes);
        textureArrowDown = textureArrowDownRes->getObject();

        phi::resource<phi::texture>* textureArrowUpRes;
        phi::importer::importTexture(phi::path::combine(directory, "arrow_up.texture"), textureArrowUpRes);
        textureArrowUp = textureArrowUpRes->getObject();

        phi::resource<phi::texture>* textureModelsRes;
        phi::importer::importTexture(phi::path::combine(directory, "models.texture"), textureModelsRes);
        textureModels = textureModelsRes->getObject();

        phi::resource<phi::texture>* textureMaterialsRes;
        phi::importer::importTexture(phi::path::combine(directory, "materials.texture"), textureMaterialsRes);
        textureMaterials = textureMaterialsRes->getObject();
    }

    void uiRepository::loadFonts(std::string directory)
    {
        auto consolasPath = phi::path::combine(directory, "Consola.ttf");
        auto arialPath = phi::path::combine(directory, "Arial.ttf");

        //fontConsolas14 = new font("Consola_14", consolasPath, 14, renderingSystem::freeTypeLibrary);
        //fontConsolas16 = new font("Consola_16", consolasPath, 16, renderingSystem::freeTypeLibrary);
        //fontConsolas18 = new font("Consola_18", consolasPath, 18, renderingSystem::freeTypeLibrary);
        //fontConsolas24 = new font("Consola_24", consolasPath, 24, renderingSystem::freeTypeLibrary);
        //fontConsolas100 = new font("Consola_100", consolasPath, 100, renderingSystem::freeTypeLibrary);
        //fontArial72 = new font("Arial_72", arialPath, 72, renderingSystem::freeTypeLibrary);
    }

    void uiRepository::loadCursors(std::string directory)
    {
        cursorText = new cursor(path::combine(directory, "text.texture"), vec2(0.5f, 0.5f));
        cursorDefault = new cursor(path::combine(directory, "default.texture"), vec2(0.0f, 0.0f));
    }
}