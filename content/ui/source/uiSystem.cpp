#include "uiSystem.h"
#include "texture.h"
#include "font.h"
#include "renderingSystem.h"

namespace phi
{
    resourcesRepository* uiSystem::repository;

    void uiSystem::init(uiSystemInfo info)
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

        if (!renderingSystem::initialized)
        {
            renderingSystemInfo renderingInfo = renderingSystemInfo();
            renderingInfo.applicationPath = info.applicationPath;
            renderingInfo.size = info.size;
            renderingSystem::init(renderingInfo);
        }
    }
}