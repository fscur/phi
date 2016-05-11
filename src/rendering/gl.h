#pragma once
#include <phi.h>

#include <core\material.h>

#include "renderingApi.h"
#include "texturesManager.h"
#include "shadersManager.h"
#include "fontsManager.h"

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
            int16_t swapInterval;
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
                int16_t swapInterval = 1,
                bool useBindlessTextures = false,
                bool useSparseTextures = false) :
                clearColor(clearColor),
                culling(culling),
                depthTest(depthTest),
                depthMask(depthMask),
                frontFace(frontFace),
                cullFace(cullFace),
                swapInterval(swapInterval),
                useBindlessTextures(useBindlessTextures),
                useSparseTextures(useSparseTextures)
            {}
        };

        struct glInfo
        {
            gl::state state;
            string shadersPath;
            string fontsPath;

            glInfo()
            {
            }
        };

    private:
        const string BINDLESS_TEXTURE_EXTENSION = "GL_ARB_bindless_texture";
        const string SPARSE_TEXTURE_EXTENSION = "GL_ARB_sparse_texture";
        const string SWAP_CONTROL_EXTENSION = "WGL_EXT_swap_control";

        static bool _initialized;

    private:
        void initOpenGLExtensions();
        void initState();
        void createDefaultResources(bool hasSparseTextures);
        texture* createDefaultTexture(bool hasSparseTextures, vec4 color);
        material* createDefaultMaterial();

    public:
        texturesManager* texturesManager;
        shadersManager* shadersManager;
        fontsManager* fontsManager;
        gl::state currentState;
        map<string, bool> extensions;
        material* defaultMaterial;
        texture* defaultAlbedoTexture;
        texture* defaultNormalTexture;
        texture* defaultSpecularTexture;
        texture* defaultEmissiveTexture;
        font* defaultFont;

    public:
        RENDERING_API gl(gl::glInfo initInfo);
        RENDERING_API ~gl();

        string getVendor() { return string((char*)glGetString(GL_VENDOR)); };
        string getRenderer() { return string((char*)glGetString(GL_RENDERER)); };
        string getVersion() { return string((char*)glGetString(GL_VERSION)); };
        RENDERING_API static void SyncPipeline();
    };
}
