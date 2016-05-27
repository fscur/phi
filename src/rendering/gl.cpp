#include <precompiled.h>
#include "gl.h"

#include <loader\importer.h>

#include "glDebugger.h"

namespace phi
{
    bool gl::_initialized = false;

    gl::gl(gl::glInfo info)
    {
        assert(!_initialized);
        _initialized = true;

        initOpenGLExtensions();

#if _DEBUG
        glDebugger::enable();
#endif

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
        safeDelete(defaultAlbedoImage);
        safeDelete(defaultNormalImage);
        safeDelete(defaultSpecularImage);
        safeDelete(defaultEmissiveImage);
        safeDelete(defaultMaterial);
    }

    void gl::initOpenGLExtensions()
    {
        auto i = 0;
        string currentExtension;
        vector<string> glExtensions;
        auto glExtension = glGetStringi(GL_EXTENSIONS, i++);

        do
        {
            currentExtension = string((char*)glExtension);
            glExtensions.push_back(currentExtension);
            glExtension = glGetStringi(GL_EXTENSIONS, i++);
        } while (glExtension != NULL && !currentExtension.empty());
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

    image* gl::createDefaultMaterialImage(const bool& hasSparseTextures, const vec4& color)
    {
        auto w = 1u;
        auto h = 1u;

        if (hasSparseTextures)
        {
            w = 128u;
            h = 128u;
        }

        auto data = new byte[w * h * 4];

        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h * 4; y += 4)
            {
                auto index = (y + h * x * 4);

                data[index + 0] = static_cast<byte>(color.r * 255);
                data[index + 1] = static_cast<byte>(color.g * 255);
                data[index + 2] = static_cast<byte>(color.b * 255);
                data[index + 3] = static_cast<byte>(color.a * 255);
            }
        }

		return new image(w, h, imageDataFormat::rgba, imageDataType::ubyte_dataType, data);
    }

    material* gl::createDefaultMaterial()
    {
        return new material(
			defaultAlbedoImage,
			defaultNormalImage,
			defaultSpecularImage,
			defaultEmissiveImage,
            vec3(1.0f),
            vec3(1.0f),
            vec3(1.0f),
            0.1f,
            0.0f,
            0.0f,
            1.0f);
    }

    void gl::createDefaultResources(const bool& hasSparseTextures)
    {
        defaultAlbedoImage = createDefaultMaterialImage(hasSparseTextures, vec4(1.0f));
        defaultNormalImage = createDefaultMaterialImage(hasSparseTextures, vec4(0.5f, 0.5f, 1.0f, 1.0f));
        defaultSpecularImage = createDefaultMaterialImage(hasSparseTextures, vec4(1.0f));
        defaultEmissiveImage = createDefaultMaterialImage(hasSparseTextures, vec4(0.0f));
        defaultMaterial = createDefaultMaterial();
        defaultFont = fontsManager->load("Consolas.ttf", 14);
    }

    void gl::SyncPipeline()
    {
        glFinish();
    }
}