#include <precompiled.h>
#include "layer.h"

namespace phi
{
    using namespace phi;

    layer::layer(camera* camera) :
        _camera(camera),
        _root(new node("root"))
    {
        createFrameUniforms();
        trackNode(_root);
    }

    layer::~layer()
    {
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

    void layer::addOnNodeSelectionChanged(std::function<void(node*)> onNodeSelectionChanged)
    {
        _onNodeSelectionChanded.push_back(onNodeSelectionChanged);
    }

    void layer::addMouseController(iMouseController* controller)
    {
        _controllers.push_back(controller);
    }

    //TODO: keep a list of dirty nodes to prevent traversing and some hard to fix buggy bugs
    //TODO: after this fix, grouping should work correctly
    //TODO: nodes pointer should be deleted on deleteSceneObjectCommand
    void layer::trackNode(node* node)
    {
        auto childAddedToken = node->childAdded.assign(std::bind(&layer::nodeChildAdded, this, std::placeholders::_1));
        auto childRemovedToken = node->childRemoved.assign(std::bind(&layer::nodeChildRemoved, this, std::placeholders::_1));
        auto transformChangedToken = node->transformChanged.assign(std::bind(&layer::nodeTransformChanged, this, std::placeholders::_1));
        auto selectionChangedToken = node->selectionChanged.assign(std::bind(&layer::nodeSelectionChanged, this, std::placeholders::_1));

        _nodeTokens[node] = new nodeEventTokens(
            childAddedToken,
            childRemovedToken,
            transformChangedToken,
            selectionChangedToken);
    }

    void layer::untrackNode(node* node)
    {
        node->childAdded.unassign(_nodeTokens[node]->childAdded);
        node->childRemoved.unassign(_nodeTokens[node]->childRemoved);
        node->transformChanged.unassign(_nodeTokens[node]->transformChanged);
        node->selectionChanged.unassign(_nodeTokens[node]->selectionChanged);

        safeDelete(_nodeTokens[node]);
        _nodeTokens.erase(node);
    }

    void layer::nodeChildAdded(node* addedChild)
    {
        addedChild->traverse([&](phi::node* node)
        {
            trackNode(node);
            for (auto onNodeAddedFunction : _onNodeAdded)
                onNodeAddedFunction(node); //TODO: do with events
        });
    }

    void layer::nodeChildRemoved(node* removedChild)
    {
    }

    void layer::nodeTransformChanged(node* changedNode)
    {
    }

    void layer::nodeSelectionChanged(node* node)
    {
        for (auto& onNodeSelectionChanged : _onNodeSelectionChanded)
            onNodeSelectionChanged(node);
    }

    void layer::add(node* node)
    {
        _root->addChild(node);
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
}
