#include <precompiled.h>
#include "glassyControlRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>

#include <rendering\texturesManager.h>

namespace phi
{
    glassyControlRenderAdapter::glassyControlRenderAdapter(resolution resolution) :
        _resolution(resolution)
    {
        createBuffers();
    }

    glassyControlRenderAdapter::~glassyControlRenderAdapter()
    {
        safeDelete(_modelMatricesBuffer);
        safeDelete(_controlsRenderDataBuffer);
    }

    void glassyControlRenderAdapter::createBuffers()
    {
        _vao = new vertexArrayObject();

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto controlQuad = geometry::create(vertices, indices);

        vector<vertexBufferAttribute> vboAttribs;
        vboAttribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        auto vbo = new vertexBuffer("vbo", vboAttribs);
        vbo->storage(controlQuad->vboSize, controlQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto ebo = new buffer("ebo", bufferTarget::element);
        ebo->storage(controlQuad->eboSize, controlQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer("modelMatrices", modelMatricesAttribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        _vao->add(vbo);
        _vao->setEbo(ebo);
        _vao->add(_modelMatricesBuffer);

        _vao->setOnRender([=]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_instances.size()));
        });

        _controlsRenderDataBuffer = new buffer("ControlRenderDataBuffer", bufferTarget::shader);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
    }

    void glassyControlRenderAdapter::updateModelMatrix(control* control)
    {
        auto instance = _instances[control];
        auto modelMatrix = control->getNode()->getTransform()->getModelMatrix();
        instance->modelMatrix = modelMatrix;

        auto offset = instance->bufferOffset * sizeof(mat4);
        _modelMatricesBuffer->subData(offset, sizeof(mat4), &modelMatrix);
    }

    void glassyControlRenderAdapter::updateControlRenderData(control * control)
    {
        auto instance = _instances[control];
        auto renderData = controlRenderData::from(control);
        instance->renderData = renderData;

        auto offset = instance->bufferOffset * sizeof(controlRenderData);
        _controlsRenderDataBuffer->subData(offset, sizeof(controlRenderData), &renderData);
    }

    void glassyControlRenderAdapter::add(control* control)
    {
        auto renderData = controlRenderData::from(control);
        auto instance = new controlInstance();
        instance->bufferOffset = _instances.size();
        instance->modelMatrix = control->getNode()->getTransform()->getModelMatrix();
        instance->renderData = renderData;

        _instances[control] = instance;

        _modelMatrices.push_back(instance->modelMatrix);
        _renderData.push_back(instance->renderData);

        _modelMatricesBuffer->data(sizeof(mat4) * _instances.size(), &_modelMatrices[0], bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData) * _instances.size(), &_renderData[0], bufferDataUsage::dynamicDraw);
    }

    void glassyControlRenderAdapter::remove(control* control)
    {
        throw new notImplementedException();
    }

    void glassyControlRenderAdapter::update(control* control)
    {
        updateModelMatrix(control);
        updateControlRenderData(control);
    }
}