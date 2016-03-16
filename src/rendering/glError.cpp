#include <precompiled.h>
#include "glError.h"

namespace phi
{
    string glError::getErrorString(GLenum error)
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

    void glError::check()
    {
#ifdef _DEBUG
        GLenum error = glGetError();

        if (error != GL_NO_ERROR)
            throw getErrorString(error);
#endif
    }
}
