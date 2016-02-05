#ifndef _PHI_EXPORTER_H_
#define _PHI_EXPORTER_H_

#include "loader.h"

#include <phi\core\geometry.h>

#include <string>
#include <vector>

namespace phi
{
    class exporter
    {
    public:
        LOADER_API static int exportMesh(std::vector<geometry*>* geometry, char* fileName);
    };
}

#endif