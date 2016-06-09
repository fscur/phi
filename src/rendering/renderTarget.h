#pragma once
#include <phi.h>
#include "textureAddress.h"

namespace phi
{
    struct renderTarget
    {
        renderTarget(
            const string& name,
            GLint w, 
            GLint h,
            const textureAddress& textureAddress) :
            name(name),
            w(w),
            h(h),
            textureAddress(textureAddress)
        {
        }

        const string name;
        GLint w;
        GLint h;
        textureAddress textureAddress;
    };
}