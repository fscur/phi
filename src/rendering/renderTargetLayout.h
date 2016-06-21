#pragma once
#include <phi.h>
#include "textureLayout.h"

namespace phi
{
    struct renderTargetLayout
    {
        renderTargetLayout(
            const string& name,
            const textureLayout& textureLayout) :
            name(name),
            textureLayout(textureLayout)
        {
        }

        const string name;
        const textureLayout textureLayout;
    };
}
