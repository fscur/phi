#pragma once
#include <phi.h>
#include "textureAddress.h"

namespace phi
{
    struct renderTarget
    {
        renderTarget(
            GLenum attachment,
            GLint w, 
            GLint h,
            textureAddress textureAddress) :
            attachment(attachment),
            w(w),
            h(h),
            textureAddress(textureAddress)
        {
        }

        GLenum attachment;
        GLint w;
        GLint h;
        textureAddress textureAddress;
    };
}