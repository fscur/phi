#include <precompiled.h>
#include "instancedVao.h"

#include <core\geometry.h>

namespace phi
{
    instancedVao::instancedVao()
    {
        create();
    }

    instancedVao::~instancedVao()
    {
        safeDelete(_modelMatricesBuffer);
    }

    void instancedVao::update(const vector<mat4>& modelMatrices)
    {
        _instances = static_cast<GLsizei>(modelMatrices.size());
        _modelMatricesBuffer->data(sizeof(mat4) * _instances, &modelMatrices[0], bufferDataUsage::dynamicDraw);
    }

    void instancedVao::create()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };
        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto controlQuad = geometry::create(vertices, indices);

        glCreateVertexArrays(1, &_id);
        glBindVertexArray(_id);

        vector<vertexAttrib> controlsVboAttribs;
        controlsVboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        controlsVboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        auto controlsVbo = new vertexBuffer(controlsVboAttribs);
        controlsVbo->storage(controlQuad->vboSize, controlQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto controlsEbo = new buffer(bufferTarget::element);
        controlsEbo->storage(controlQuad->eboSize, controlQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexAttrib> controlsModelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            controlsModelMatricesAttribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer(controlsModelMatricesAttribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        glBindVertexArray(0);
    }

    void instancedVao::render()
    {
        glBindVertexArray(_id);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _instances);
        glBindVertexArray(0);
    }
}