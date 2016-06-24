#include <precompiled.h>
#include "vertexArrayObject.h"

#include <core\geometry.h>

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

    void vertexArrayObject::add(iVertexBuffer* buffer)
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

    vertexArrayObject* vertexArrayObject::createPostProcessVao()
    {
        auto quad = geometry::createQuad(2.0f);
        auto renderFunction = [] { glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); };
        return createQuadVao(quad, renderFunction);
    }

    vertexArrayObject* vertexArrayObject::createQuadVao(geometry* quad, std::function<void(void)> renderFunction)
    {
        vector<vertexBufferAttribute> attribs;
        attribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto vbo = new vertexBuffer("vbo", attribs);
        vbo->storage(quad->vboSize, quad->vboData, bufferStorageUsage::write);

        auto ebo = new buffer("ebo", bufferTarget::element);
        ebo->storage(quad->eboSize, quad->eboData, bufferStorageUsage::write);

        auto quadVao = new vertexArrayObject();
        quadVao->add(vbo);
        quadVao->setEbo(ebo);
        quadVao->setOnRender(renderFunction);

        return quadVao;
    }
}