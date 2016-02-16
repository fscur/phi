#ifndef _PHI_GL_H_
#define _PHI_GL_H_

#include "rendering.h"
#include <core\globals.h>

#include <vector>
#include <map>
#include <string>

#include <GL\glew.h>

namespace phi
{
    class gl
    {
    public:
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

        struct state
        {
        public:
            vec4 clearColor;
            bool culling;
            bool depthTest;
            bool depthMask;
            frontFace frontFace;
            cullFace cullFace;
            bool useBindlessTextures;
            bool useSparseTextures;

        public:
            state(
                vec4 clearColor = vec4(0.0f),
                bool culling = true,
                bool depthTest = true,
                bool depthMask = true,
                gl::frontFace frontFace = gl::frontFace::ccw,
                gl::cullFace cullFace = gl::cullFace::back,
                bool useBindlessTextures = false,
                bool useSparseTextures = false):
                clearColor(clearColor),
                culling(culling),
                depthTest(depthTest),
                depthMask(depthMask),
                frontFace(frontFace),
                cullFace(cullFace),
                useBindlessTextures(useBindlessTextures),
                useSparseTextures(useSparseTextures)
            {}
        };
    private:
        static bool _initialized;

    private:
        static void printOpenGLDetails();
        static void initOpenGLExtensions();
        static void initState();
    public:
        RENDERING_API static gl:: state* currentState;
        RENDERING_API static std::map<std::string, bool> extensions;
        RENDERING_API static void init(gl::state state);
    };
}

#endif
