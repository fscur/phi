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
    _materialsRepository = new phi::resourcesRepository<phi::material>();
    loadMaterials(_resourcesPath);
    phi::log(std::to_string(_materialsRepository->getAllResources().size()));
}

//void resourcesLoader::loadMaterials()
//{
//    _materialsRepository = new resourcesRepository();
//    auto sourceDirectory = _resourcesPath + "\\" + _materialsPath;
//    loadMaterials(sourceDirectory, sourceDirectory);
//}

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
            if (phi::importer::importMaterial(file.path, matRes))
            {
                _materialsRepository->addResource(matRes);
            }
        }
    }
}

void library::loadTextures(std::string directory)
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
            if (phi::importer::importMaterial(file.path, matRes))
            {
                _materialsRepository->addResource(matRes);
            }
        }
    }
}