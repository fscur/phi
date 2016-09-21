#include <precompiled.h>
#include "controlRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>
#include <rendering\texturesManager.h>

namespace phi
{
    controlRenderAdapter::controlRenderAdapter(camera* camera) :
        _vao(nullptr),
        _modelMatricesBuffer(nullptr),
        _renderDataBuffer(nullptr),
        _controls(vector<control*>()),
        _camera(camera),
        _cameraChangedEventToken()
    {
        _cameraChangedEventToken = _camera->getTransform()->getChangedEvent()->assign(std::bind(&controlRenderAdapter::onCameraChanged, this, std::placeholders::_1));
        createVao();
        createControlRenderDataBuffer();
    }

    void controlRenderAdapter::createVao()
    {
        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<control*, mat4>("modelMatrices", modelMatricesAttribs);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        auto controlQuad = createControlQuad();

        _vao = vertexArrayObject::createQuadVao(controlQuad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(controlQuad);
    }

    geometry* controlRenderAdapter::createControlQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        return geometry::create(vertices, indices);
    }

    void controlRenderAdapter::createControlRenderDataBuffer()
    {
        _renderDataBuffer = new mappedBuffer<control*, controlRenderData>("ControlRenderDataBuffer", bufferTarget::shader);
    }

    controlRenderAdapter::~controlRenderAdapter()
    {
        safeDelete(_renderDataBuffer);
        safeDelete(_vao);
    }

    void controlRenderAdapter::add(control* control)
    {
        auto renderData = controlRenderData::from(control);
        _renderDataBuffer->add(control, renderData);

        auto modelMatrix = getModelMatrix(control);

        _modelMatricesBuffer->add(control, modelMatrix);
        _controls.push_back(control);
    }

    mat4 controlRenderAdapter::getModelMatrix(control* control)
    {
        mat4 modelMatrix;
        if (control->isBillboard())
        {
            auto controlTransform = control->getNode()->getTransform();
            auto position = controlTransform->getPosition();
            auto scale = controlTransform->getSize();
            auto cameraTransform = _camera->getTransform();
            auto cameraUp = cameraTransform->getUp();
            auto cameraRight = cameraTransform->getRight();
            auto cameraDir = cameraTransform->getDirection();

            auto rotationMatrix = mat4(
                vec4(-cameraRight, 0.0f),
                vec4(cameraUp, 0.0f),
                vec4(-cameraDir, 0.0f),
                vec4(vec3(), 1.0f));

            auto scaleMatrix = glm::scale(scale);
            auto translationMatrix = glm::translate(position);
            modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        }
        else
            modelMatrix = control->getNode()->getTransform()->getModelMatrix();

        return modelMatrix;
    }

    void controlRenderAdapter::remove(control* control)
    {
        _renderDataBuffer->remove(control);
        _modelMatricesBuffer->remove(control);
        phi::removeIfContains(_controls, control);
    }

    void controlRenderAdapter::update(control* control)
    {
        updateModelMatrix(control);
        updateControlRenderData(control);
    }

    void controlRenderAdapter::updateModelMatrix(control* control)
    {
        auto modelMatrix = getModelMatrix(control);
        _modelMatricesBuffer->update(control, modelMatrix);
    }

    void controlRenderAdapter::updateControlRenderData(control* control)
    {
        auto renderData = controlRenderData::from(control);
        _renderDataBuffer->update(control, renderData);
    }

    void controlRenderAdapter::onCameraChanged(transform* transform)
    {
        for (auto control : _controls)
            updateModelMatrix(control);
    }
}