#pragma once
#include <phi.h>

#include <core/guid.h>

#include "componentJson.h"

namespace phi
{
    struct meshComponentJson : public componentJson
    {
        guid materialResourceGuid;
        guid geometryResourceGuid;
    };
}