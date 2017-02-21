#include <precompiled.h>
#include "gl.h"

#include <loader\importer.h>

#include "texturesManager.h"
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

        texturesManager::initialize(hasBindlessTextures, hasSparseTextures);
        fontsManager::initialize(info.fontsPath);
    }

    gl::~gl()
    {
        texturesManager::release();
        fontsManager::release();
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

        if (currentState.culling)
            glEnable(GL_CULL_FACE);

        glCullFace(currentState.cullFace);
        glFrontFace(currentState.frontFace);

        if (currentState.depthTest)
            glEnable(GL_DEPTH_TEST);

        GLboolean depthMask = currentState.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);

        if (extensions[SWAP_CONTROL_EXTENSION])
            wglSwapIntervalEXT(currentState.swapInterval);
    }

    void gl::syncPipeline()
    {
        glFinish();
    }

    void gl::resize(const resolution& resolution)
    {
        auto width = static_cast<GLsizei>(resolution.width);
        auto height = static_cast<GLsizei>(resolution.height);

        glViewport(0, 0, width, height);
    }
}