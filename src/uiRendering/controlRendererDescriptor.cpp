#include <precompiled.h>
#include "controlRendererDescriptor.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>

namespace phi
{
    controlRendererDescriptor::controlRendererDescriptor(gl * gl) :
        _gl(gl)
    {
        createBuffers();
    }

    controlRendererDescriptor::~controlRendererDescriptor()
    {
        for (auto pair : _instances)
            safeDelete(pair.second);

        safeDelete(_controlsRenderDataBuffer);
    }

    void controlRendererDescriptor::createBuffers()
    {
        _vao = new instancedVao();

        _controlsRenderDataBuffer = new buffer(bufferTarget::shader);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
    }

    void controlRendererDescriptor::add(control* control)
    {
        auto texture = texturesManager::getTextureFromImage(control->getBackgroundImage(), _gl->defaultAlbedoImage);
        auto address = _gl->texturesManager->get(texture);

        auto renderData = controlRenderData();
        renderData.backgroundColor = control->getBackgroundColor();
        renderData.backgroundTextureUnit = address.unit;
        renderData.backgroundTexturePage = address.page;

        auto instance = new controlInstance();
        instance->bufferOffset = _instances.size();
        instance->modelMatrix = control->getNode()->getTransform()->getModelMatrix();
        instance->renderData = renderData;

        _instances[control] = instance;

        _modelMatrices.push_back(instance->modelMatrix);
        _renderData.push_back(instance->renderData);

        _vao->update(_modelMatrices);

        _controlsRenderDataBuffer->data(sizeof(controlRenderData) * _instances.size(), &_renderData[0], bufferDataUsage::dynamicDraw);
    }

    void controlRendererDescriptor::remove(control* control)
    {
        throw new notImplementedException();
    }

    void controlRendererDescriptor::update(control* control)
    {
        throw new notImplementedException();
    }
}