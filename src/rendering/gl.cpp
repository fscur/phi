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

        phi::log("Initializing OpenGL.");

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

        phi::log("Vendor: " + print(GL_VENDOR) + ".");
        phi::log("Renderer: " + print(GL_RENDERER) + ".");
        phi::log("Version: " + print(GL_VERSION) + ".");
    }

    void gl::initOpenGLExtensions()
    {
        const GLubyte* glExtension = nullptr;
        auto i = 0;

        glExtension = glGetStringi(GL_EXTENSIONS, i);
        std::vector<std::string> glExtensions;

        while (glExtension != NULL)
        {
            glExtensions.push_back(std::string((char*)glExtension));
            glExtension = glGetStringi(GL_EXTENSIONS, ++i);
        }

        //cleaning error from loading the last (inexistent) extension
        glGetError();

        std::vector<std::string> phiExtensions;
        phiExtensions.push_back("GL_ARB_bindless_texture");
        phiExtensions.push_back("GL_ARB_sparse_texture");

        phi::log("Extensions:");

        for (auto phiExtension : phiExtensions)
        {
            auto found = phi::contains(glExtensions, phiExtension);
            extensions[phiExtension] = found;
            phi::log(phiExtension + (found ? "[Ok]" : "[Not Ok]"));
        }
    }

    void gl::initState()
    {
        auto state = *currentState;

        glClearColor(state.clearColor.r, state.clearColor.g, state.clearColor.b, state.clearColor.a);

        if (state.culling)
            glEnable(GL_CULL_FACE);

        glCullFace(state.cullFace);
        glFrontFace(state.frontFace);

        if (state.depthTest)
            glEnable(GL_DEPTH_TEST);

        auto depthMask = state.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);
    }

    std::string gl::getErrorString(GLenum error)
    {
        switch (error)
        {
        case GL_NO_ERROR: return "No error";
            break;
        case GL_INVALID_ENUM: return "Invalid enum";
            break;
        case GL_INVALID_VALUE: return "Invalid value";
            break;
        case GL_INVALID_OPERATION: return "Invalid operation";
            break;
        case GL_OUT_OF_MEMORY: return "Out of memory";
            break;
        default:
            break;
        }

        return std::string();
    }

    void gl::printError(std::string msg)
    {
        auto error = glGetError();
        phi::log(msg + " (" + getErrorString(error) + ")");
    }
}