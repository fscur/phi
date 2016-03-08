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
        auto importTextureFunction = [](phi::string filePath, phi::resource<phi::texture>*& resource)
        {
            return phi::importer::importTexture(filePath, resource);
        };

        _texturesRepository =
            load<phi::texture>(_libraryPath + "/textures", ".texture", importTextureFunction);

        auto importMaterialFunction = [&](phi::string filePath, phi::resource<phi::material>*& resource)
        {
            return phi::importer::importMaterial(filePath, resource, _texturesRepository);
        };

        _materialsRepository = load<phi::material>(_libraryPath + "/materials", ".material", importMaterialFunction);

        auto importModelFunction = [&](phi::string filePath, phi::resource<phi::node>*& resource)
        {
            return phi::importer::importNode(filePath, resource, _materialsRepository);
        };

        _nodesRepository = load<phi::node>(_libraryPath + "/models", ".model", importModelFunction);
    }
}