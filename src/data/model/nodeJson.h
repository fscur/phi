#pragma once
#include <phi.h>

#include "componentJson.h"

namespace phi
{
    struct nodeJson
    {
        vector<componentJson> components;
        vector<nodeJson> children;
    };
}