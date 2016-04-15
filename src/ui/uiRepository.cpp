#include <precompiled.h>
#include "uiRepository.h"

#include <io\path.h>

#include <loader\importer.h>

#include <rendering\texture.h>
#include <rendering\font.h>

#include "cursor.h"

namespace phi
{
    font* uiRepository::fontConsolas14;

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
        loadFonts(phi::path::combine(info.resourcesPath, UI_FONTS_PATH), info.fontManager);
        loadTextures(phi::path::combine(info.resourcesPath, UI_IMAGES_PATH));
        loadCursors(phi::path::combine(info.resourcesPath, UI_CURSORS_PATH));

        initialized = true;
    }

    void uiRepository::loadTextures(string directory)
    {
        //TODO: Find a way to deal with resource<T> delete. Maybe in this case should import a .png, not a texture
        //TODO: try catch this shit
        resource<texture>* textureButtonRes = importer::importTexture(path::combine(directory, "button.texture"));
        textureButton = textureButtonRes->getObject();

        resource<texture>* textureArrowDownRes = importer::importTexture(path::combine(directory, "arrow_down.texture"));
        textureArrowDown = textureArrowDownRes->getObject();

        resource<texture>* textureArrowUpRes = importer::importTexture(path::combine(directory, "arrow_up.texture"));
        textureArrowUp = textureArrowUpRes->getObject();

        resource<texture>* textureModelsRes = importer::importTexture(path::combine(directory, "models.texture"));
        textureModels = textureModelsRes->getObject();

        resource<texture>* textureMaterialsRes = importer::importTexture(path::combine(directory, "materials.texture"));
        textureMaterials = textureMaterialsRes->getObject();
    }

    void uiRepository::loadFonts(string directory, fontManager* fontManager)
    {
        auto consolasPath = path::combine(directory, "Consola.ttf");
        auto arialPath = path::combine(directory, "Arial.ttf");
        
        fontConsolas14 = new font(consolasPath, 14);
    }

    void uiRepository::loadCursors(string directory)
    {
        cursorText = new cursor(path::combine(directory, "text.texture"), vec2(0.5f, 0.5f));
        cursorDefault = new cursor(path::combine(directory, "default.texture"), vec2(0.0f, 0.0f));
    }
}