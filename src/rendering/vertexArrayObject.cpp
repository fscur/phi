#include <precompiled.h>
#include "vertexArrayObject.h"

namespace phi
{
    vertexArrayObject::vertexArrayObject() :
        _id(0)
    {
        glCreateVertexArrays(1, &_id);
    }

    vertexArrayObject::~vertexArrayObject()
    {
        for (auto& vbo : _vbos)
            safeDelete(vbo);

        safeDelete(_ebo);
    }

    void vertexArrayObject::add(vertexBuffer* buffer)
    {
        glBindVertexArray(_id);
        buffer->initialize();
        glBindVertexArray(0);

        _vbos.push_back(buffer);
    }

    void vertexArrayObject::setEbo(buffer* buffer)
    {
        _ebo = buffer;

        glBindVertexArray(_id);
        _ebo->bind();
        glBindVertexArray(0);
    }

    void vertexArrayObject::render()
    {
        glBindVertexArray(_id);
        _onRender();
        glBindVertexArray(0);
    }
}