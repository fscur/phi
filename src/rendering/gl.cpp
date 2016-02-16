#include "gl.h"

namespace phi
{
    std::map<std::string, bool> gl::extensions = std::map<std::string, bool>();
    gl::state* gl::currentState = nullptr;
    bool gl::_initialized = false;

    void gl::init(gl::state state)
    {
        if (_initialized)
            return;

        phi::log("initializing OpenGL.");

        printOpenGLDetails();
        initOpenGLExtensions();

        auto hasBindlessTextures = extensions["GL_ARB_bindless_texture"];
        auto hasSparseTextures = extensions["GL_ARB_sparse_texture"];

        currentState = new gl::state(
            state.clearColor,
            state.culling,
            state.depthTest,
            state.depthMask,
            state.frontFace,
            state.cullFace,
            state.useBindlessTextures && hasBindlessTextures,
            state.useSparseTextures && hasSparseTextures);

        initState();

        _initialized = true;
    }

    void gl::printOpenGLDetails()
    {
        auto print = [](GLenum s) -> std::string {
            return std::string((char*)glGetString(s));
        };

        phi::log("vendor: " + print(GL_VENDOR) + ".");
        phi::log("renderer: " + print(GL_RENDERER) + ".");
        phi::log("version: " + print(GL_VERSION) + ".");
    }

    void gl::initOpenGLExtensions()
    {
        const GLubyte* glExtension = nullptr;
        auto i = 0;

        glExtension = glGetStringi(GL_EXTENSIONS, i++);
        std::vector<std::string> glExtensions;

        while (glExtension != NULL)
        {
            glExtensions.push_back(std::string((char*)glExtension));
            glExtension = glGetStringi(GL_EXTENSIONS, i++);
        }

        std::vector<std::string> phiExtensions;
        phiExtensions.push_back("GL_ARB_bindless_texture");
        phiExtensions.push_back("GL_ARB_sparse_texture");

        phi::log("extensions:");

        for (auto phiExtension : phiExtensions)
        {
            auto found = phi::contains(glExtensions, phiExtension);
            extensions[phiExtension] = found;
            phi::log(phiExtension + (found ? "[true]" : "[false]"));
        }
    }

    void gl::initState()
    {
        auto state = *currentState;

        glClearColor(state.clearColor.r, state.clearColor.g, state.clearColor.b, state.clearColor.a);

        if (state.culling)
            glEnable(GL_CULL_FACE);

        auto cullBackFace = state.cullFace == phi::gl::cullFace::back;
        glCullFace(cullBackFace ? GL_BACK : GL_FRONT);

        auto frontFaceCCW = state.frontFace == phi::gl::frontFace::ccw;
        glFrontFace(frontFaceCCW ? GL_CCW : GL_CW);

        if (state.depthTest)
            glEnable(GL_DEPTH_TEST);

        auto depthMask = state.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);
    }
}