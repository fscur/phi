#include "vertexArrayObject.h"

namespace phi
{
    void vertexArrayObject::createVbo(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), 0));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), sizeof(float) * 3));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 5));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 8));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _buffers.push_back(new vertexBuffer(info, attribs));
    }

    void vertexArrayObject::createMaterialsIdsBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _buffers.push_back(new vertexBuffer(info, attribs));
    }

    void vertexArrayObject::createModelMatricesBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; i++)
            attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(glm::mat4), sizeof(GLfloat) * i * 4, 1));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _buffers.push_back(new vertexBuffer(info, attribs));
    }

    void vertexArrayObject::createEbo(void* data, GLsizeiptr size)
    {
        auto info = elementBufferDataInfo(data, size, GL_STATIC_DRAW);
        _ebo = new elementBuffer(info);
    }
}