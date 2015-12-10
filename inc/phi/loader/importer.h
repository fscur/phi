#ifndef _PHI_IMPORTER_H_
#define _PHI_IMPORTER_H_

#include "loader.h"

#include <phi/core/geometryData.h>

#include <string>
#include <vector>

namespace phi
{
    class importer
    {
    public:
        LOADER_API static int importMesh(std::string fileName, std::vector<geometryData*>* data);
    };
}

#endif