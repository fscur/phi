#include <precompiled.h>
#include "gl.h"

#include <loader\importer.h>

namespace phi
{
    bool gl::_initialized = false;

    gl::gl(gl::glInfo info)
    {
        assert(!_initialized);

        _initialized = true;

        initOpenGLExtensions();

        auto hasBindlessTextures = extensions[BINDLESS_TEXTURE_EXTENSION] && info.state.useBindlessTextures;
        auto hasSparseTextures = extensions[SPARSE_TEXTURE_EXTENSION] && info.state.useSparseTextures;

        currentState = gl::state(
            info.state.clearColor,
            info.state.culling,
            info.state.depthTest,
            info.state.depthMask,
            info.state.frontFace,
            info.state.cullFace,
            info.state.swapInterval,
            hasBindlessTextures,
            hasSparseTextures);

        initState();
        
        texturesManager = new phi::texturesManager(hasBindlessTextures, hasSparseTextures);
        shadersManager = new phi::shadersManager(info.shadersPath);
        fontsManager = new phi::fontsManager(info.fontsPath, texturesManager);

        createDefaultResources(hasSparseTextures);
    }

    gl::~gl()
    {
        safeDelete(texturesManager);
        safeDelete(shadersManager);
        safeDelete(fontsManager);
        safeDelete(defaultAlbedoTexture);
        safeDelete(defaultNormalTexture);
        safeDelete(defaultSpecularTexture);
        safeDelete(defaultEmissiveTexture);
        safeDelete(defaultMaterial);
    }

    void gl::initOpenGLExtensions()
    {
        const GLubyte* glExtension = nullptr;
        auto i = 0;

        glExtension = glGetStringi(GL_EXTENSIONS, i);
        vector<string> glExtensions;

        while (glExtension != NULL)
        {
            glExtensions.push_back(string((char*)glExtension));
            glExtension = glGetStringi(GL_EXTENSIONS, ++i);
        }
        glGetError(); //cleaning error from loading the last (inexistent) extension

        vector<string> phiExtensions =
        {
            BINDLESS_TEXTURE_EXTENSION,
            SPARSE_TEXTURE_EXTENSION,
            SWAP_CONTROL_EXTENSION,
        };

        for (auto phiExtension : phiExtensions)
        {
            auto found = phi::contains(glExtensions, phiExtension);
            extensions[phiExtension] = found;
        }
    }

    void gl::initState()
    {
        glClearColor(currentState.clearColor.r, currentState.clearColor.g, currentState.clearColor.b, currentState.clearColor.a);
        glError::check();

        if (currentState.culling)
        {
            glEnable(GL_CULL_FACE);
            glError::check();
        }

        glCullFace(currentState.cullFace);
        glError::check();

        glFrontFace(currentState.frontFace);
        glError::check();

        if (currentState.depthTest)
        {
            glEnable(GL_DEPTH_TEST);
            glError::check();
        }

        GLboolean depthMask = currentState.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);
        glError::check();

        if (extensions[SWAP_CONTROL_EXTENSION])
        {
            wglSwapIntervalEXT(currentState.swapInterval);
            glError::check();
        }
    }

    texture* gl::createDefaultTexture(bool hasSparseTextures, vec4 color)
    {
        auto x = 1u;
        auto y = 1u;

        if (hasSparseTextures)
        {
            x = 128u;
            y = 128u;
        }

        auto data = new byte[x * y * 4];

        for (size_t i = 0; i < x; ++i)
        {
            for (size_t j = 0; j < y * 4; j += 4)
            {
                auto index = (j + y * i * 4);

                data[index + 0] = static_cast<byte>(color.r * 255);
                data[index + 1] = static_cast<byte>(color.g * 255);
                data[index + 2] = static_cast<byte>(color.b * 255);
                data[index + 3] = static_cast<byte>(color.a * 255);
            }
        }

        return new texture(
            x,
            y,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data,
            GL_REPEAT,
            GL_NEAREST,
            GL_NEAREST,
            false);
    }

    material* gl::createDefaultMaterial()
    {
        return new material(
            defaultAlbedoTexture,
            defaultNormalTexture,
            defaultSpecularTexture,
            defaultEmissiveTexture,
            vec3(1.0f),
            vec3(1.0f),
            vec3(1.0f),
            0.1f,
            0.0f,
            0.0f,
            1.0f);
    }

    void gl::createDefaultResources(bool hasSparseTextures)
    {
        defaultAlbedoTexture = createDefaultTexture(hasSparseTextures, vec4(1.0f));
        defaultNormalTexture = createDefaultTexture(hasSparseTextures, vec4(0.5f, 0.5f, 1.0f, 1.0f));
        defaultSpecularTexture = createDefaultTexture(hasSparseTextures, vec4(1.0f));
        defaultEmissiveTexture = createDefaultTexture(hasSparseTextures, vec4(0.0f));
        defaultMaterial = createDefaultMaterial();
        defaultFont = fontsManager->load("Consolas.ttf", 14);
    }

    void gl::SyncPipeline()
    {
        glFinish();
    }
}