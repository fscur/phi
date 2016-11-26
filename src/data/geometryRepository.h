#pragma once
#include <phi.h>

#include <core/geometry.h>

#include "dataApi.h"
#include "abstractions/iGeometryRepository.h"

#include "model/index.h"

namespace phi
{
    class geometryRepository : public iGeometryRepository
    {
    public:
        DATA_API geometryRepository(string libraryPath);
        DATA_API ~geometryRepository();

        DATA_API virtual geometry* getById(const guid& guid) override;

    private:
        const string GEOMETRIES_INDEX_FILE_NAME = "geometries.index";
        geometry* loadGeometry(string fileName);

    private:
        string _libraryPath;
        index _index;
    };
}