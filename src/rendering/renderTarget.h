#pragma once
#include <phi.h>
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
        phi::texture* texture;
    public:
        renderTarget(
            GLenum attachment,
            GLint w, 
            GLint h,
            phi::textureAddress textureAddress,
            phi::texture* texture) :
            attachment(attachment),
            w(w),
            h(h),
            textureAddress(textureAddress),
            texture(texture)
        {
        }

        ~renderTarget()
        {
            safeDelete(texture);
        }
    };
}