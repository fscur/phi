#include <precompiled.h>
#include "library.h"

#include <loader\importer.h>
#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(string resourcesPath) :
        _libraryPath(resourcesPath)
    {
    }

    library::~library()
    {
        safeDelete(_imagesRepository);
        safeDelete(_materialsRepository);
        safeDelete(_geometriesRepository);
        safeDelete(_nodesRepository);
    }

    void library::init()
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
            return importer::loadNode(filePath, _materialsRepository, _geometriesRepository);
        };

        _imagesRepository = load<image>(_libraryPath + "/textures", ".texture", importTextureFunction);
        _geometriesRepository = load<geometry>(_libraryPath + "/models", ".geometry", importGeometryFunction);
        _materialsRepository = load<material>(_libraryPath + "/materials", ".material", importMaterialFunction);
        _nodesRepository = load<node>(_libraryPath + "/models", ".model", importModelFunction);
        debug(_("Library initialized."));
    }
}