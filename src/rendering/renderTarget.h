#pragma once
#include <phi.h>
#include "textureLayout.h"
#include "textureAddress.h"

namespace phi
{
    struct renderTarget
    {
        renderTarget(
            const string& name,
            GLint w, 
            GLint h,
            const textureLayout& layout,
            const textureAddress& textureAddress) :
            name(name),
            w(w),
            h(h),
            layout(layout),
            textureAddress(textureAddress)
        {
        }

        const string name;
        GLint w;
        GLint h;
        textureLayout layout;
        textureAddress textureAddress;
    };
}