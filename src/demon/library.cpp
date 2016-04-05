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
        safeDelete(_nodesRepository);
    }

    void library::init()
    {
        auto importTextureFunction = [](string filePath)
        {
            return importer::importTexture(filePath);
        };

        auto importMaterialFunction = [&](string filePath)
        {
            return importer::importMaterial(filePath, _texturesRepository);
        };

        auto importModelFunction = [&](string filePath)
        {
            return importer::importNode(filePath, _materialsRepository);
        };

        _texturesRepository = load<texture>(_libraryPath + "/textures", ".texture", importTextureFunction);
        _materialsRepository = load<material>(_libraryPath + "/materials", ".material", importMaterialFunction);
        _nodesRepository = load<node>(_libraryPath + "/models", ".model", importModelFunction);
        debug(_("Library initialized."));
    }
}