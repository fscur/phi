#include <core\globals.h>
#include <demo\library.h>
#include <loader\importer.h>
#include <io\path.h>

library::library(std::string resourcesPath)
    : _resourcesPath(resourcesPath)
{
}

void library::init()
{
    _texturesRepository = new phi::resourcesRepository<phi::texture>();
    _materialsRepository = new phi::resourcesRepository<phi::material>();
    _objectsRepository = new phi::resourcesRepository<phi::object3D>();

    loadTextures(_resourcesPath + "/textures");
    loadMaterials(_resourcesPath + "/materials");
    loadObjects(_resourcesPath + "/models");
}

void library::loadTextures(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadTextures(dir.path);

    auto filters = std::vector<std::string>();
    filters.push_back(".texture");

    auto files = phi::path::getFiles(directory, filters);
    for (auto &file : files)
    {
        phi::resource<phi::texture>* texRes;

        if (phi::importer::importTexture(file.path, texRes))
            _texturesRepository->addResource(texRes);
    }
}

void library::loadMaterials(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadMaterials(dir.path);

    auto filters = std::vector<std::string>();
    filters.push_back(".material");

    auto files = phi::path::getFiles(directory,filters);
    for (auto &file : files)
    {
        phi::resource<phi::material>* matRes;

        if (phi::importer::importMaterial(file.path, matRes, _texturesRepository))
            _materialsRepository->addResource(matRes);
    }
}

void library::loadObjects(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadObjects(dir.path);

    auto filters = std::vector<std::string>();
    filters.push_back(".model");

    auto files = phi::path::getFiles(directory, filters);

    for (auto &file : files)
    {
        phi::resource<phi::object3D>* objRes;

        if (phi::importer::importObject3D(file.path, objRes, _materialsRepository))
            _objectsRepository->addResource(objRes);
    }
}