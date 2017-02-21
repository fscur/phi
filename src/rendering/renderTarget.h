#pragma once
#include <phi.h>
#include "texture.h"

namespace phi
{
    struct renderTarget
    {
        renderTarget(
            const string& name,
            texture* texture) :
            name(name),
            texture(texture)
        {
        }

        const string name;
        texture* texture;
    };
}