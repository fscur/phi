#include <precompiled.h>
#include "gl.h"

namespace phi
{
    bool gl::_initialized = false;

    gl::gl(gl::glInfo info)
    {
        assert(!_initialized);

        _initialized = true;

        phi::logInfo("Initializing OpenGL.");

        printOpenGLDetails();
        initOpenGLExtensions();

        auto hasBindlessTextures = extensions["GL_ARB_bindless_texture"] && info.state.useBindlessTextures;
        auto hasSparseTextures = extensions["GL_ARB_sparse_texture"] && info.state.useSparseTextures;

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
        delete texturesManager;
        delete shadersManager;
    }

    void gl::printOpenGLDetails()
    {
        auto print = [](GLenum s) -> string {
            return string((char*)glGetString(s));
        };

        phi::logInfo("Vendor: " + print(GL_VENDOR) + ".");
        phi::logInfo("Renderer: " + print(GL_RENDERER) + ".");
        phi::logInfo("Version: " + print(GL_VERSION) + ".");
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

        //cleaning error from loading the last (inexistent) extension
        glGetError();

        vector<string> phiExtensions;
        phiExtensions.push_back("GL_ARB_bindless_texture");
        phiExtensions.push_back("GL_ARB_sparse_texture");

        //phi::logInfo("Extensions:");

        for (auto phiExtension : phiExtensions)
        {
            auto found = phi::contains(glExtensions, phiExtension);
            extensions[phiExtension] = found;
            phi::logInfo(phiExtension + (found ? "[Ok]" : "[Not Ok]"));
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

        auto depthMask = currentState.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);
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

    string gl::getErrorString(GLenum error)
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

        return string();
    }

    void gl::printError(string msg)
    {
        auto error = getErrorString(glGetError());
        phi::logInfo(msg + " (" + error + ")");
    }
}