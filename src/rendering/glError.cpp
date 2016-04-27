#include <precompiled.h>
#include "glError.h"
#include <core\exception.h>

namespace phi
{
    string glError::getErrorString(GLenum error)
    {
        switch (error)
        {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
        }
    }

    void glError::check()
    {
#ifdef _DEBUG
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            auto msg = getErrorString(error);
            std::cout << msg << std::endl;
            //throw exception(msg);
        }
#endif
    }
}
