#pragma once
#include <precompiled.h>
#include "rendering.h"
#include "material.h"
#include "texturesManager.h"
#include "shadersManager.h"

namespace phi
{
    class gl
    {
    public:
        enum frontFace
        {
            cw = GL_CW,
            ccw = GL_CCW
        };

        enum cullFace
        {
            front = GL_FRONT,
            back = GL_BACK
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

        struct glInfo
        {
            public:
            gl::state state;
            string shadersPath;
        public:
            glInfo() {}
        };

    private:
        static bool _initialized;

    private:
        void printOpenGLDetails();
        void initOpenGLExtensions();
        void initState();
        void initDefaultResources(bool sparse);
        texture* createDefaultTexture(bool sparse, vec4 color);
        void createDefaultMaterial();

    public:
        phi::texturesManager* texturesManager;
        phi::shadersManager* shadersManager;
        gl:: state currentState;
        std::map<string, bool> extensions;
        material* defaultMaterial;
        texture* defaultAlbedoTexture;
        texture* defaultNormalTexture;
        texture* defaultSpecularTexture;
        texture* defaultEmissiveTexture;
    public:
        RENDERING_API gl(gl::glInfo initInfo);
        RENDERING_API ~gl();
        RENDERING_API string getErrorString(GLenum error);
        RENDERING_API void printError(string msg);
    };
}
