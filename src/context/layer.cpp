#include <precompiled.h>
#include "layer.h"

namespace phi
{
    using namespace phi;

    layer::layer(camera* camera, vector<renderPass*>& renderPasses) :
        _camera(camera),
        _renderPasses(renderPasses),
        _root(new node("root"))
    {
        initialize();
    }

    layer::layer(camera* camera, phi::vector<phi::renderPass*>&& renderPasses) :
        _camera(camera),
        _renderPasses(renderPasses),
        _root(new node("root"))
    {
        initialize();
    }

    void layer::initialize()
    {
        createFrameUniforms();

        auto cameraNode = new node("camera");
        cameraNode->addComponent(_camera);
        //add(cameraNode);
    }

    void layer::createFrameUniforms()
    {
        _frameUniformsBuffer = new buffer<frameUniformBlock>(bufferTarget::uniform);

        auto frameUniform = frameUniformBlock(_camera->getProjectionMatrix(), _camera->getViewMatrix());
        _frameUniformsBuffer->storage(sizeof(frameUniformBlock), &frameUniform, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void layer::updateFrameUniforms()
    {
        auto frameUniform = phi::frameUniformBlock(_camera->getProjectionMatrix(), _camera->getViewMatrix());
        _frameUniformsBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniform);
    }

    layer::~layer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void layer::add(node* node)
    {
        _root->addChild(node);
        _onNodeAdded(node); //TODO: do with events
    }

    void layer::update()
    {
        updateFrameUniforms();

        for (auto renderPass : _renderPasses)
            renderPass->update();
    }
    
    void layer::render()
    {
        _frameUniformsBuffer->bindBufferBase(0);

        for (auto renderPass : _renderPasses)
            renderPass->render();
    }
}
