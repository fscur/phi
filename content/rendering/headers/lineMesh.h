#ifndef _PHI_LINE_MESH_H_
#define _PHI_LINE_MESH_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include <vector>
#include <glm/glm.hpp>
#include "rendering.h"

namespace phi
{
    class lineMesh
    {
    private:
        GLuint _vao;
        GLuint _positionsVbo;
        GLuint _indicesVbo;

        GLfloat* _positionsBuffer;
        GLuint* _indicesBuffer;

        GLuint _indicesCount;

        GLuint _pSize;
        GLuint _iSize;

        std::vector<glm::vec3> _positions;
        std::vector<GLuint> _indices;

    private:
        void storeBuffers();

    protected:
        lineMesh();
        void addData(std::vector<glm::vec3> positions, std::vector<GLuint> indices);

    public:

        RENDERING_API ~lineMesh();

        RENDERING_API static lineMesh* create(std::string name, std::vector<glm::vec3> &positions, std::vector<GLuint> &indices);
        RENDERING_API static lineMesh* create(
            GLuint positionsCount,
            GLfloat* positionsBuffer,
            GLuint indicesCount,
            GLuint* indicesBuffer);

        RENDERING_API void bind();
        RENDERING_API void render();
        RENDERING_API void unbind();
    };
}

#endif