#include <precompiled.h>
#include "library.h"

#include <loader\importer.h>
#include <io\path.h>

namespace demon
{
    library::library(phi::gl* gl, phi::string resourcesPath) :
        _gl(gl),
        _libraryPath(resourcesPath)
    {
        phi::importer::defaultAlbedoTexture = _gl->defaultAlbedoTexture;
        phi::importer::defaultNormalTexture = _gl->defaultNormalTexture;
        phi::importer::defaultSpecularTexture = _gl->defaultSpecularTexture;
        phi::importer::defaultEmissiveTexture = _gl->defaultEmissiveTexture;
        phi::importer::defaultMaterial = _gl->defaultMaterial;
    }

    void library::init()
    {
        auto importTextureFunction = [](phi::string filePath)
        {
            return phi::importer::importTexture(filePath);
        };

        auto importMaterialFunction = [&](phi::string filePath)
        {
            return phi::importer::importMaterial(filePath, _texturesRepository);
        };

        auto importModelFunction = [&](phi::string filePath)
        {
            return phi::importer::importNode(filePath, _materialsRepository);
        };

        _texturesRepository = load<phi::texture>(_libraryPath + "/textures", ".texture", importTextureFunction);
        _materialsRepository = load<phi::material>(_libraryPath + "/materials", ".material", importMaterialFunction);
        _nodesRepository = load<phi::node>(_libraryPath + "/models", ".model", importModelFunction);
    }
}