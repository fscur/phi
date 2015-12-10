#include <phi/core/globals.h>
#include <phi/io/path.h>
#include <phi/rendering/geometry.h>

#include <phi/core/globals.h>

#include <phi/io/path.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace phi
{
    geometry::geometry(geometryData* data)
    {
        _vao = 0;

        _positionsVbo = 0;
        _texCoordsVbo = 0;
        _normalsVbo = 0;
        _tangentsVbo = 0;
        _indicesVbo = 0;
        _isBound = false;
        _data = data;

        storeBuffers();
    }

    geometry::~geometry()
    {
        glDeleteBuffers(1, &_positionsVbo);
        glDeleteBuffers(1, &_texCoordsVbo);
        glDeleteBuffers(1, &_normalsVbo);
        glDeleteBuffers(1, &_tangentsVbo);
        glDeleteBuffers(1, &_indicesVbo);
        glDeleteVertexArrays(1, &_vao);
    }

    void geometry::storeBuffers()
    {
        _indicesCount = _data->getIndicesCount();

        auto positionsBuffer = _data->getPositionsBuffer();
        auto texCoordsBuffer = _data->getTexCoordsBuffer();
        auto normalsBuffer = _data->getNormalsBuffer();
        auto tangentsBuffer = _data->getTangentsBuffer();
        auto pSize = _data->getPSize();
        auto tSize = _data->getTSize();
        auto nSize = _data->getNSize();
        auto tgSize = _data->getTGSize();
        auto indices = _data->getIndices();
        auto indicesSize = _indicesCount * sizeof(GLuint);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_positionsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glBufferData(GL_ARRAY_BUFFER, pSize, positionsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &_texCoordsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
        glBufferData(GL_ARRAY_BUFFER, tSize, texCoordsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &_normalsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
        glBufferData(GL_ARRAY_BUFFER, nSize, normalsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &_tangentsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _tangentsVbo);
        glBufferData(GL_ARRAY_BUFFER, tgSize, tangentsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &_indicesVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &(*indices)[0], GL_STATIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &_indices[0], GL_STATIC_DRAW); // Ativar arte abstrata
    }

   void geometry::bind()
    {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
        glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
        glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
        glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, _tangentsVbo);
        glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
        _isBound = true;
    }

    void geometry::render()
    {
        glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0);
    }

    void geometry::unbind()
    {
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        _isBound = false;
    }
}
