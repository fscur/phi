#include <precompiled.h>
#include "layer.h"

namespace phi
{
    using namespace phi;

    layer::layer(camera* camera) :
        _camera(camera),
        _root(new node("root"))
    {
        initialize();
    }

    layer::~layer()
    {
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
        _frameUniformsBuffer = new buffer("FrameUniformsDataBuffer", bufferTarget::uniform);

        auto frameUniform = frameUniformBlock(_camera->getProjectionMatrix(), _camera->getViewMatrix());
        _frameUniformsBuffer->storage(sizeof(frameUniformBlock), &frameUniform, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void layer::updateFrameUniforms()
    {
        auto frameUniform = phi::frameUniformBlock(_camera->getProjectionMatrix(), _camera->getViewMatrix());
        _frameUniformsBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniform);
    }

    void layer::addOnUpdate(std::function<void(void)> updateFunction)
    {
        _onUpdate.push_back(updateFunction);
    }

    void layer::addRenderPass(renderPass* renderPass)
    {
        _renderPasses.push_back(renderPass);
    }

    void layer::addOnNodeAdded(std::function<void(node*)> onNodeAdded)
    {
        _onNodeAdded.push_back(onNodeAdded);
    }

    void layer::addMouseController(iMouseController* controller)
    {
        _controllers.push_back(controller);
    }

    void layer::add(node* node)
    {
        _root->addChild(node);

        node->traverse([&](phi::node* currentNode)
        {
            for (auto onNodeAddedFunction : _onNodeAdded)
                onNodeAddedFunction(currentNode); //TODO: do with events
        });
    }

    void layer::update()
    {
        for (auto& controller : _controllers)
            controller->update();

        updateFrameUniforms();

        for (auto updateFunction : _onUpdate)
            updateFunction();
    }

    void layer::render()
    {
        _frameUniformsBuffer->bindBufferBase(0);

        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    void layer::onMouseDown(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onMouseDown(e);
    }

    void layer::onMouseMove(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onMouseMove(e);
    }

    void layer::onMouseUp(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onMouseUp(e);
    }

    void layer::onMouseWheel(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onMouseWheel(e);
    }

    void layer::onKeyDown(keyboardEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onKeyDown(e);
    }

    void layer::onKeyUp(keyboardEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onKeyUp(e);
    }

    void layer::resize(const resolution& resolution)
    {
        for (auto& renderPass : _renderPasses)
            renderPass->resize(resolution);
    }
}
