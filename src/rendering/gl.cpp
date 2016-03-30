#include <precompiled.h>
#include "gl.h"

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
            hasBindlessTextures,
            hasSparseTextures);

        initState();

        texturesManager = new phi::texturesManager(hasBindlessTextures, hasSparseTextures);
        shadersManager = new phi::shadersManager(info.shadersPath);

        initDefaultResources(hasSparseTextures);
    }

    gl::~gl()
    {
        delete defaultMaterial;
        delete texturesManager;
        delete shadersManager;
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
    }

    void gl::initDefaultResources(bool sparse)
    {
        defaultAlbedoTexture = createDefaultTexture(sparse, vec4(1.0f));
        defaultNormalTexture = createDefaultTexture(sparse, vec4(0.5f, 0.5f, 1.0f, 1.0f));
        defaultSpecularTexture = createDefaultTexture(sparse, vec4(1.0f));
        defaultEmissiveTexture = createDefaultTexture(sparse, vec4(0.0f));

        createDefaultMaterial();
    }

    void gl::createDefaultMaterial()
    {
        defaultMaterial = new material(
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

    texture* gl::createDefaultTexture(bool sparse, vec4 color)
    {
        auto x = 1;
        auto y = 1;

        if (sparse)
        {
            x = 128;
            y = 128;
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

        auto texture = new phi::texture(
            x,
            y,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data);

        return texture;
    }
}