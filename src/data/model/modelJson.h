#pragma once
#include <phi.h>

#include <core/guid.h>

#include "nodeJson.h"

namespace phi
{
    struct modelJson
    {
        guid guid;
        nodeJson node;
    };
}