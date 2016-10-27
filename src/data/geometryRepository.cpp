#include <precompiled.h>

#include <io/path.h>

#include "importResourceException.h"
#include "geometryRepository.h"

namespace phi
{
    using namespace cereal;

    geometryRepository::geometryRepository(string libraryPath) :
        _libraryPath(libraryPath)
    {
        auto indexPath = path::combine(_libraryPath, GEOMETRIES_INDEX_FILE_NAME);
        _index = index::load(indexPath);
    }

    geometryRepository::~geometryRepository()
    {
    }

    geometry* geometryRepository::getById(const guid& guid)
    {
        auto entry = _index.getEntryById(guid);
        auto geometryPath = path::combine(_libraryPath, entry.path);

        return loadGeometry(geometryPath);
    }

    geometry* geometryRepository::loadGeometry(string fileName)
    {
#ifdef _WIN32
        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            throw importResourceException("File coult not be loaded.", fileName);
        }

        auto geometryGuidBytes = new uint8_t[16];
        file.read(reinterpret_cast<char*>(geometryGuidBytes), 16);

        int verticesCount = -1;
        file.read(reinterpret_cast<char*>(&verticesCount), sizeof(int));

        auto positionsBuffer = new float[verticesCount * 3];
        auto texCoordsBuffer = new float[verticesCount * 2];
        auto normalsBuffer = new float[verticesCount * 3];

        auto positionsSize = verticesCount * 3 * sizeof(float);
        auto texCoordsSize = verticesCount * 2 * sizeof(float);
        auto normalsSize = verticesCount * 3 * sizeof(float);

        file.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
        file.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
        file.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

        int indicesCount = -1;
        file.read(reinterpret_cast<char*>(&indicesCount), sizeof(int));

        auto indicesBuffer = new uint[indicesCount];
        file.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * sizeof(int));

        file.close();

        auto name = path::getFileNameWithoutExtension(fileName);
        auto guid = phi::guid(geometryGuidBytes);
        safeDelete(geometryGuidBytes);

        auto geometry = geometry::create(
            guid,
            verticesCount,
            positionsBuffer,
            texCoordsBuffer,
            normalsBuffer,
            indicesCount,
            indicesBuffer);

        safeDeleteArray(positionsBuffer);
        safeDeleteArray(texCoordsBuffer);
        safeDeleteArray(normalsBuffer);
        safeDeleteArray(indicesBuffer);

        return geometry;
#else
        throw importResourceException("importGeometry was not implemented ins other platforms than WIN32");
#endif
    }
}