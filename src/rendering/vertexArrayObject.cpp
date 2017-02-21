#include <precompiled.h>
#include "vertexArrayObject.h"

#include <core\geometry.h>
#include "vertexBuffer.h"
#include "indexBuffer.h"

namespace phi
{
    vertexArrayObject::vertexArrayObject() :
        _id(0)
    {
        glCreateVertexArrays(1, &_id);
    }

    vertexArrayObject::~vertexArrayObject()
    {
        for (auto& buffer : _buffers)
            safeDelete(buffer);

        glDeleteVertexArrays(1, &_id);
    }

    void vertexArrayObject::addBuffer(iVertexBuffer* buffer)
    {
        glBindVertexArray(_id);
        buffer->initialize();
        glBindVertexArray(0);

        _buffers.push_back(buffer);
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
        auto quadVao = createQuadVao(quad, renderFunction);
        safeDelete(quad);

        return quadVao;
    }

    vertexArrayObject* vertexArrayObject::createQuadVao(geometry* quad, std::function<void(void)> renderFunction)
    {
        vector<vertexBufferAttribute> attribs;
        attribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto vbo = new vertexBuffer("vbo", attribs);
        vbo->storage(quad->vboSize, quad->vboData, bufferStorageUsage::write);

        auto ebo = new indexBuffer("ebo");
        ebo->storage(quad->eboSize, quad->eboData, bufferStorageUsage::write);

        auto quadVao = new vertexArrayObject();
        quadVao->addBuffer(vbo);
        quadVao->addBuffer(ebo);
        quadVao->setOnRender(renderFunction);

        return quadVao;
    }

    vertexArrayObject* vertexArrayObject::createBoxVao(geometry* box, std::function<void(void)> renderFunction)
    {
        vector<vertexBufferAttribute> attribs;
        attribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        attribs.push_back(vertexBufferAttribute(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));

        auto vbo = new vertexBuffer("vbo", attribs);
        vbo->storage(box->vboSize, box->vboData, bufferStorageUsage::write);

        auto ebo = new indexBuffer("ebo");
        ebo->storage(box->eboSize, box->eboData, bufferStorageUsage::write);

        auto quadVao = new vertexArrayObject();
        quadVao->addBuffer(vbo);
        quadVao->addBuffer(ebo);
        quadVao->setOnRender(renderFunction);

        return quadVao;
    }
}