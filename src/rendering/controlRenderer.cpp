#include <precompiled.h>
#include "controlRenderer.h"

#include <core\vertex.h>
#include <core\geometry.h>

namespace phi
{
    controlRenderer::controlRenderer(gl* gl) :
        _gl(gl),
        _controlsCount(0)
    {
        createBuffers();
    }

    controlRenderer::~controlRenderer()
    {
    }

    void controlRenderer::createBuffers()
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
        
        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        
        vector<vertexAttrib> controlsVboAttribs;
        controlsVboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        controlsVboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        auto controlsVbo = new vertexBuffer<vertex>(controlsVboAttribs);
        controlsVbo->storage(controlQuad->vboSize, controlQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto controlsEbo = new buffer<uint>(bufferTarget::element);
        controlsEbo->storage(controlQuad->eboSize, controlQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);
        
        vector<vertexAttrib> controlsModelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            controlsModelMatricesAttribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _controlsModelMatricesBuffer = new vertexBuffer<mat4>(controlsModelMatricesAttribs);
        _controlsModelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);
        
        glBindVertexArray(0);
        
        _controlsRenderDataBuffer = new buffer<controlRenderData>(bufferTarget::shader);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
    }

    void controlRenderer::add(controlRenderData* renderData, transform* transform)
    {
        _controlsCount++;

        _modelMatrices.push_back(transform->getModelMatrix());
        _renderData.push_back(*renderData);

        _controlsModelMatricesBuffer->data(sizeof(mat4) * _controlsCount, &_modelMatrices[0], bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData) * _controlsCount, &_renderData[0], bufferDataUsage::dynamicDraw);
    }

    void controlRenderer::render(shader* shader)
    {
        _controlsRenderDataBuffer->bindBufferBase(1);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        shader->bind();
        shader->setUniform(0, _gl->texturesManager->units);

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_controlsCount));
        glBindVertexArray(0);

        shader->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }
}