#include "uiRepository.h"
#include "renderingSystem.h"
#include "texture.h"
#include "font.h"
#include "cursor.h"
#include <glm\glm.hpp>

namespace phi
{
    resourcesRepository* uiRepository::repository;
    bool uiRepository::initialized = false;

    void uiRepository::init(uiRepositoryInfo info)
    {
        repository = new resourcesRepository();

        std::string texturesPath = info.applicationPath + UI_TEXTURES_PATH;
        repository->addResource(texture::fromFile(texturesPath + "button.png"));

        std::string fontsPath = info.applicationPath + UI_FONTS_PATH;
        repository->addResource(new font("Consola_14", fontsPath + "Consola.ttf", 14, renderingSystem::freeTypeLibrary));
        repository->addResource(new font("Consola_16", fontsPath + "Consola.ttf", 16, renderingSystem::freeTypeLibrary));
        repository->addResource(new font("Consola_18", fontsPath + "Consola.ttf", 18, renderingSystem::freeTypeLibrary));
        repository->addResource(new font("Consola_24", fontsPath + "Consola.ttf", 24, renderingSystem::freeTypeLibrary));
        repository->addResource(new font("Consola_100", fontsPath + "Consola.ttf", 100, renderingSystem::freeTypeLibrary));
        repository->addResource(new font("Arial_72", fontsPath + "Arial.ttf", 72, renderingSystem::freeTypeLibrary));
        
        std::string cursorsPath = info.applicationPath + UI_CURSORS_PATH;
        repository->addResource(new cursor("TextCursor", cursorsPath + "text.png", glm::vec2(0.5f, 0.5f)));
        repository->addResource(new cursor("DefaultCursor", cursorsPath + "default.png", glm::vec2(0.0f, 0.0f)));

        initialized = true;
    }
}