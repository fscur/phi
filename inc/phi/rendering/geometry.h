#ifndef _PHI_GEOMETRY_H_
#define _PHI_GEOMETRY_H_

#include "rendering.h"

#include <phi/core/geometryData.h>

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <string>
#include <list>
#include <vector>

namespace phi
{
    class geometry
    {
    private:

        GLuint _vao;
        GLuint _positionsVbo;
        GLuint _texCoordsVbo;
        GLuint _normalsVbo;
        GLuint _tangentsVbo;
        GLuint _indicesVbo;
        GLuint _indicesCount;

        geometryData* _data;

        bool _isBound;
        bool _isLoadedOnGpu;

    private:
        void storeBuffers();

    public:
        RENDERING_API geometry(geometryData* data);
        RENDERING_API ~geometry();

        RENDERING_API bool getIsBound() const { return _isBound; }

        RENDERING_API void bind();
        RENDERING_API void render();
        RENDERING_API void unbind();
        RENDERING_API void loadToGpu();
        RENDERING_API void releaseFromGpu();
    };
}
#endif