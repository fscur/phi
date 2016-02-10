#ifndef _PHI_OPENGL_H_
#define _PHI_OPENGL_H_

#include <phi\core\globals.h>

namespace phi
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

    struct glConfig
    {
        vec4 clearColor;
        bool culling;
        bool depthTest;
        bool depthMask;
        frontFace frontFace;
        cullFace cullFace;
    };
}

#endif