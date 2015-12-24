#include <phi\core\globals.h>
#include <phi\demo\library.h>
#include <phi\loader\importer.h>
#include <phi\io\path.h>

library::library(std::string resourcesPath)
    : _resourcesPath(resourcesPath)
{
}

void library::init()
{
    _texturesRepository = new phi::resourcesRepository<phi::texture>();
    _materialsRepository = new phi::resourcesRepository<phi::material>();
    _objectsRepository = new phi::resourcesRepository<phi::object3D>();

    loadTextures(_resourcesPath);
    loadMaterials(_resourcesPath);
    loadObjects(_resourcesPath);
}

void library::loadTextures(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadTextures(dir.path);

    auto files = phi::path::getFiles(directory);
    for (auto &file : files)
    {
        if (phi::path::getExtension(file.path) == ".texture")
        {
            phi::resource<phi::texture>* texRes;
            if (phi::importer::importTexture(file.path, texRes))
                _texturesRepository->addResource(texRes);
        }
    }
}

void library::loadMaterials(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadMaterials(dir.path);

    auto files = phi::path::getFiles(directory);
    for (auto &file : files)
    {
        if (phi::path::getExtension(file.path) == ".material")
        {
            phi::resource<phi::material>* matRes;
            if (phi::importer::importMaterial(file.path, matRes, _texturesRepository))
                _materialsRepository->addResource(matRes);
        }
    }
}

void library::loadObjects(std::string directory)
{
    auto subDirs = phi::path::getDirectories(directory);
    for (auto &dir : subDirs)
        loadObjects(dir.path);

    auto files = phi::path::getFiles(directory);
    for (auto &file : files)
    {
        if (phi::path::getExtension(file.path) == ".model")
        {
            phi::resource<phi::object3D>* objRes;
            if (phi::importer::importObject3D(file.path, objRes, _materialsRepository))
                _objectsRepository->addResource(objRes);
        }
    }
}