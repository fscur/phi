#include <precompiled.h>
#include "library.h"

#include <loader\importer.h>
#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(string resourcesPath) :
        _libraryPath(resourcesPath),
		_imagesRepository(new phi::resourcesRepository<image>()),
		_materialsRepository(new phi::resourcesRepository<material>()),
		_geometriesRepository(new phi::resourcesRepository<geometry>()),
		_nodesRepository(new phi::resourcesRepository<node>())
    {
    }

    library::~library()
    {
        safeDelete(_imagesRepository);
        safeDelete(_materialsRepository);
        safeDelete(_geometriesRepository);
        safeDelete(_nodesRepository);
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
			return importer::loadNode(filePath, _materialsRepository, _geometriesRepository);
		};

		load<image>(_libraryPath + "/textures", { ".texture" }, _imagesRepository, importTextureFunction);
		load<geometry>(_libraryPath + "/models", { ".geometry" }, _geometriesRepository, importGeometryFunction);
		load<material>(_libraryPath + "/materials", { ".material" }, _materialsRepository, importMaterialFunction);
		load<node>(_libraryPath + "/models", { ".model" }, _nodesRepository, importModelFunction);

        debug(_("Library initialized."));
    }
}