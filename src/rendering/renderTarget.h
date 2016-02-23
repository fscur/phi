#pragma once

#include "textureAddress.h"
#include "texture.h"

namespace phi
{
    class renderTarget
    {
    public:
        GLenum attachment;
        GLint w;
        GLint h;
        phi::textureAddress textureAddress;
    public:
        renderTarget(
            GLenum attachment,
            GLint w, 
            GLint h,
            phi::textureAddress textureAddress) :
            attachment(attachment),
            w(w),
            h(h),
            textureAddress(textureAddress)
        {
        }
    };
}