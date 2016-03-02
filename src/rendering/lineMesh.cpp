#include "lineMesh.h"

namespace phi
{
    lineMesh::lineMesh()
    {
        _vao = 0;

        _positionsVbo = 0;
        _indicesVbo = 0;
        _indicesCount = 0;
    }

    lineMesh::~lineMesh()
    {
        delete[] _positionsBuffer;

        glDeleteBuffers(1, &_positionsVbo);
        glDeleteBuffers(1, &_indicesVbo);
        glDeleteVertexArrays(1, &_vao);
    }

    lineMesh* lineMesh::create(std::string name, std::vector<vec3> &positions, std::vector<GLuint>* indices)
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
        auto indices = new std::vector<GLuint>();

        for (GLuint i = 0; i < indicesCount; ++i)
            indices->push_back(indicesBuffer[i]);

        std::vector<vec3> positions;

        for (GLuint i = 0; i < positionsCount; ++i)
        {
            float x = positionsBuffer[i * 3 + 0];
            float y = positionsBuffer[i * 3 + 1];
            float z = positionsBuffer[i * 3 + 2];

            positions.push_back(vec3(x, y, z));
        }

        lineMesh* m = new lineMesh();
        auto floatSize = (GLuint)sizeof(GLfloat);
        m->_indices = indices;
        m->_indicesCount = indicesCount;
        m->_positionsBuffer = positionsBuffer;
        m->_pSize = positionsCount * 3 * floatSize;

        m->storeBuffers();

        return m;
    }

    void lineMesh::addData(std::vector<vec3> positions, std::vector<GLuint>* indices)
    {
        _positions = positions;
        _indices = indices;
        _indicesCount = (GLuint)indices->size();

        _pSize = (GLuint)positions.size() * 3 * sizeof(GLfloat);

        _positionsBuffer = new GLfloat[positions.size() * 3];

        std::vector<vec3>::iterator i;
        unsigned int pIndex = 0;

        for (auto i = 0; i < positions.size(); ++i)
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
        glBindVertexArray(_vao);

        glGenBuffers(1, &_positionsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glBufferData(GL_ARRAY_BUFFER, _pSize, _positionsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &_indicesVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &(*_indices)[0], GL_STATIC_DRAW);
    }

    void lineMesh::bind()
    {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
    }

    void lineMesh::render()
    {
        glDrawElements(GL_LINES, _indicesCount, GL_UNSIGNED_INT, 0);
    }

    void lineMesh::unbind()
    {
        glDisableVertexAttribArray(0);
    }
}