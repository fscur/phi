#pragma once
#include <phi.h>
#include "renderingApi.h"

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

        vector<vec3> _positions;
        vector<GLuint>* _indices;

    private:
        void storeBuffers();

    protected:
        lineMesh();
        void addData(vector<vec3> positions, vector<GLuint>* indices);

    public:

        RENDERING_API ~lineMesh();

        RENDERING_API static lineMesh* create(string name, vector<vec3> &positions, vector<GLuint>* indices);
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