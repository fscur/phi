#ifndef _PHI_GL_CONFIG_H_
#define _PHI_GL_CONFIG_H_

#include <core\globals.h>

namespace phi
{
    namespace gl
    {
        enum frontFace
        {
            cw,
            ccw
        };

        enum cullFace
        {
            front,
            back
        };

        struct config
        {
            vec4 clearColor;
            bool culling;
            bool depthTest;
            bool depthMask;
            frontFace frontFace;
            cullFace cullFace;
        };
    }
}

#endif