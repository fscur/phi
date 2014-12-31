#include "ui.h"
#include "texture.h"
#include "font.h"

namespace phi
{
    resourcesRepository* ui::repository;

    void ui::init(std::string path)
    {
        repository = new resourcesRepository();

        std::string texturesPath = path + UI_TEXTURES_PATH;
        repository->addResource(texture::fromFile(texturesPath + "button.png"));

        std::string fontsPath = path + UI_FONTS_PATH;
        repository->addResource(new font("Consola_14", fontsPath + "Consola.ttf", 14));
        repository->addResource(new font("Consola_16", fontsPath + "Consola.ttf", 16));
        repository->addResource(new font("Consola_18", fontsPath + "Consola.ttf", 18));
        repository->addResource(new font("Consola_24", fontsPath + "Consola.ttf", 24));
        repository->addResource(new font("Consola_100", fontsPath + "Consola.ttf", 100));

        if (!rendering::initialized)
            rendering::init(path);
    }
}