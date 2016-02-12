#include <core\globals.h>
#include <demo\library.h>
#include <loader\importer.h>
#include <io\path.h>

library::library(std::string resourcesPath)
    : _libraryPath(resourcesPath)
{
}

void library::init()
{
    auto importTextureFunction = [](std::string filePath, phi::resource<phi::texture>*& resource)
    {
        return phi::importer::importTexture(filePath, resource);
    };

    _texturesRepository =
        load<phi::texture>(_libraryPath + "/textures", ".texture", importTextureFunction);

    auto importMaterialFunction = [&](std::string filePath, phi::resource<phi::material>*& resource)
    {
        return phi::importer::importMaterial(filePath, resource, _texturesRepository);
    };

    _materialsRepository = load<phi::material>(_libraryPath + "/materials", ".material", importMaterialFunction);

    auto importModelFunction = [&](std::string filePath, phi::resource<phi::object3D>*& resource)
    {
        return phi::importer::importObject3D(filePath, resource, _materialsRepository);
    };

    _objectsRepository = load<phi::object3D>(_libraryPath + "/models", ".model", importModelFunction);
}