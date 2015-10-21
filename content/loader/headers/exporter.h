#ifndef _PHI_EXPORTER_H_
#define _PHI_EXPORTER_H_

#include <string>
#include <vector>
#include "loader.h"
#include "meshData.h"

namespace phi
{
    class exporter
    {
    public:
        LOADER_API static int exportMesh(std::vector<meshData*>* data, char* fileName);
    };
}

#endif