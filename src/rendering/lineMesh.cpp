#include <precompiled.h>
#include "lineMesh.h"
#include "glError.h"

namespace phi
{
    lineMesh::lineMesh() :
        _vao(0u),
        _positionsVbo(0u),
        _indicesVbo(0u),
        _indicesCount(0u),
        _pSize(0u),
        _iSize(0u),
        _positionsBuffer(nullptr),
        _indicesBuffer(nullptr),
        _positions(vector<vec3>()),
        _indices(vector<uint>())
    {
    }

    lineMesh::~lineMesh()
    {
        delete[] _positionsBuffer;

        glDeleteBuffers(1, &_positionsVbo);
        glError::check();

        glDeleteBuffers(1, &_indicesVbo);
        glError::check();

        glDeleteVertexArrays(1, &_vao);
        glError::check();
    }

    lineMesh* lineMesh::create(string name, vector<vec3>& positions, vector<uint>& indices)
    {
        lineMesh* m = new lineMesh();
        m->addData(positions, indices);
        return m;
    }

    lineMesh* lineMesh::create(
        GLuint positionsCount,
        GLfloat* positionsBuffer,
        GLuint indicesCount,
        GLuint* indicesBuffer)
    {
        vector<uint> indices;

        for (size_t i = 0; i < indicesCount; ++i)
            indices.push_back(indicesBuffer[i]);

        vector<vec3> positions;

        for (GLuint i = 0; i < positionsCount; ++i)
        {
            float x = positionsBuffer[i * 3 + 0];
            float y = positionsBuffer[i * 3 + 1];
            float z = positionsBuffer[i * 3 + 2];

            positions.push_back(vec3(x, y, z));
        }

        lineMesh* m = new lineMesh();
        auto floatSize = sizeof(GLfloat);
        m->_indices = indices;
        m->_indicesCount = indicesCount;
        m->_positionsBuffer = positionsBuffer;
        m->_pSize = positionsCount * 3 * static_cast<GLuint>(floatSize);

        m->storeBuffers();

        return m;
    }

    void lineMesh::addData(vector<vec3>& positions, vector<uint>& indices)
    {
        size_t positionsCount = positions.size();

        _positions = positions;
        _indices = indices;
        _indicesCount = static_cast<GLuint>(indices.size());
        _pSize = static_cast<GLuint>(positionsCount) * 3 * static_cast<GLuint>(sizeof(GLfloat));
        _positionsBuffer = new GLfloat[positionsCount * 3];

        for (size_t i = 0u; i < positionsCount; ++i)
        {
            _positionsBuffer[i * 3] = positions[i].x;
            _positionsBuffer[i * 3 + 1] = positions[i].y;
            _positionsBuffer[i * 3 + 2] = positions[i].z;
        }

        storeBuffers();
    }

    void lineMesh::storeBuffers()
    {
        auto indicesSize = _indicesCount * sizeof(GLuint);

        glGenVertexArrays(1, &_vao);
        glError::check();
        
        glBindVertexArray(_vao);
        glError::check();

        glGenBuffers(1, &_positionsVbo);
        glError::check();

        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glError::check();

        glBufferData(GL_ARRAY_BUFFER, _pSize, _positionsBuffer, GL_STATIC_DRAW); 
        glError::check();

        glGenBuffers(1, &_indicesVbo);
        glError::check();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
        glError::check();

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &_indices[0], GL_STATIC_DRAW);
        glError::check();
    }

    void lineMesh::bind()
    {
        glEnableVertexAttribArray(0);
        glError::check();

        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glError::check();

        glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glError::check();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
        glError::check();
    }

    void lineMesh::render()
    {
        glDrawElements(GL_LINES, _indicesCount, GL_UNSIGNED_INT, 0);
        glError::check();
    }

    void lineMesh::unbind()
    {
        glDisableVertexAttribArray(0);
        glError::check();
    }
}