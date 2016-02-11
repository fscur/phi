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
        public:
            vec4 clearColor;
            bool culling;
            bool depthTest;
            bool depthMask;
            frontFace frontFace;
            cullFace cullFace;
            uint maxTexturePerTextureArray;
        public:
            config(
                vec4 clearColor = vec4(0.0f),
                bool culling = true,
                bool depthTest = true,
                bool depthMask = true,
                gl::frontFace frontFace = gl::frontFace::ccw,
                gl::cullFace cullFace = gl::cullFace::back,
                uint maxTexturePerTextureArray = 10) :
                clearColor(clearColor),
                culling(culling),
                depthTest(depthTest),
                depthMask(depthMask),
                frontFace(frontFace),
                cullFace(cullFace),
                maxTexturePerTextureArray(maxTexturePerTextureArray)
            {}
        };
    }
}

#endif