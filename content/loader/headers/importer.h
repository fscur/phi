#ifndef _PHI_IMPORTER_H_
#define _PHI_IMPORTER_H_

#include <string>
#include <vector>
#include "loader.h"
#include "meshData.h"

namespace phi
{
    class importer
    {
    public:
        LOADER_API static int importMesh(std::string fileName, std::vector<meshData*>* data);
    };
}

#endif