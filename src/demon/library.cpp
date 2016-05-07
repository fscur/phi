#include <precompiled.h>
#include "library.h"

#include <loader\importer.h>
#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(gl* gl, string resourcesPath) :
        _gl(gl),
        _libraryPath(resourcesPath)
    {
    }

    library::~library()
    {
        safeDelete(_texturesRepository);
        safeDelete(_materialsRepository);
        safeDelete(_geometriesRepository);
        safeDelete(_nodesRepository);
    }

    void library::init()
    {
        auto importTextureFunction = [](string filePath)
        {
            return importer::importTexture(filePath);
        };

        auto importGeometryFunction = [](string filePath)
        {
            return importer::importGeometry(filePath);
        };

        auto importMaterialFunction = [&](string filePath)
        {
            return importer::importMaterial(filePath, _texturesRepository);
        };

        auto importModelFunction = [&](string filePath)
        {
            return importer::loadNode(filePath, _materialsRepository, _geometriesRepository);
        };

        _texturesRepository = load<texture>(_libraryPath + "/textures", ".texture", importTextureFunction);
        _geometriesRepository = load<geometry>(_libraryPath + "/models", ".geometry", importGeometryFunction);
        _materialsRepository = load<material>(_libraryPath + "/materials", ".material", importMaterialFunction);
        _nodesRepository = load<node>(_libraryPath + "/models", ".model", importModelFunction);
        debug(_("Library initialized."));
    }
}