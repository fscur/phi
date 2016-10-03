#include <precompiled.h>
#include "library.h"

#include <loader\importer.h>
#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(string resourcesPath) :
        _libraryPath(resourcesPath),
        _imagesRepository(new phi::entityRepository<image>()),
        _materialsRepository(new phi::entityRepository<material>()),
        _geometriesRepository(new phi::entityRepository<geometry>()),
        _modelsRepository(new phi::entityRepository<model>())
    {
    }

    library::~library()
    {
        safeDelete(_imagesRepository);
        safeDelete(_materialsRepository);
        safeDelete(_geometriesRepository);
        safeDelete(_modelsRepository);
    }

    void library::load()
    {
        auto importTextureFunction = [](string filePath)
        {
            return importer::loadImage(filePath);
        };

        auto importGeometryFunction = [](string filePath)
        {
            return importer::loadGeometry(filePath);
        };

        auto importMaterialFunction = [&](string filePath)
        {
            return importer::loadMaterial(filePath, _imagesRepository);
        };

        auto importModelFunction = [&](string filePath)
        {
            return importer::loadModel(filePath, _materialsRepository, _geometriesRepository);
        };

        load<image>(_libraryPath + "/textures", { ".texture" }, _imagesRepository, importTextureFunction);
        load<geometry>(_libraryPath + "/models", { ".geometry" }, _geometriesRepository, importGeometryFunction);
        load<material>(_libraryPath + "/materials", { ".material" }, _materialsRepository, importMaterialFunction);
        load<model>(_libraryPath + "/models", { ".model" }, _modelsRepository, importModelFunction);

        debug(_("Library initialized."));
    }
}