#include <precompiled.h>
#include "layer.h"

#include <common/mouseInteractionComponent.h>
#include <core/time.h>

#include "pickingId.h"
namespace phi
{
    layer::layer(camera* camera) :
        _camera(camera),
        _root(new node("root")),
        _currentController(nullptr)
    {
        createFrameUniforms();
        trackNode(_root);
    }

    layer::~layer()
    {
        for (auto& onDeleteFunction : _onDelete)
            onDeleteFunction();

        for (auto& pair : _nodeTokens)
            untrackNode(pair.first);

        for (auto& mouseController : _controllers)
            safeDelete(mouseController);

        safeDelete(_frameUniformsBuffer);
        safeDelete(_root);
    }

    void layer::createFrameUniforms()
    {
        _frameUniformsBuffer = new buffer("FrameUniformsDataBuffer", bufferTarget::uniform);

        auto frameUniform = frameUniformBlock();
        frameUniform.p = _camera->getProjectionMatrix();
        frameUniform.v = _camera->getViewMatrix();
        frameUniform.ip = _camera->getInverseProjectionMatrix();
        frameUniform.vp = _camera->getViewProjectionMatrix();
        frameUniform.resolution = _camera->getResolution().toVec2();
        frameUniform.far = _camera->getFar();
        frameUniform.near = _camera->getNear();
        frameUniform.halfFovTangent = _camera->getHalfFovTangent();
        frameUniform.time = (float)time::totalSeconds;

        _frameUniformsBuffer->storage(sizeof(frameUniformBlock), &frameUniform, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void layer::trackNode(node* node)
    {
        auto childAddedToken = node->childAdded.assign(std::bind(&layer::nodeAdded, this, std::placeholders::_1));
        auto childRemovedToken = node->childRemoved.assign(std::bind(&layer::nodeRemoved, this, std::placeholders::_1));
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
    }

    void layer::updateFrameUniforms()
    {
        auto frameUniform = frameUniformBlock();
        frameUniform.p = _camera->getProjectionMatrix();
        frameUniform.v = _camera->getViewMatrix();
        frameUniform.ip = _camera->getInverseProjectionMatrix();
        frameUniform.vp = _camera->getViewProjectionMatrix();
        frameUniform.resolution = _camera->getResolution().toVec2();
        frameUniform.far = _camera->getFar();
        frameUniform.near = _camera->getNear();
        frameUniform.halfFovTangent = _camera->getHalfFovTangent();
        frameUniform.time = (float)time::totalSeconds;

        _frameUniformsBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniform);
    }

    void layer::nodeAdded(node* node)
    {
        node->traverse([&](phi::node* addedNode)
        {
            trackNode(addedNode);

            auto clickComponent = addedNode->getComponent<phi::mouseInteractionComponent>();
            if (clickComponent)
                pickingId::setNextId(clickComponent);

            for (auto onNodeAddedFunction : _onNodeAdded)
                if (onNodeAddedFunction)
                    onNodeAddedFunction(addedNode);
        });
    }

    void layer::nodeRemoved(node* node)
    {
        node->traverse([&](phi::node* removedNode)
        {
            untrackNode(removedNode);
            _nodeTokens.erase(removedNode);

            for (auto onNodeRemovedFunction : _onNodeRemoved)
                if (onNodeRemovedFunction)
                    onNodeRemovedFunction(removedNode);
        });
    }

    void layer::nodeTransformChanged(node* changedNode)
    {
        for (auto onNodeTransformChangedFunction : _onNodeTransformChanged)
            if (onNodeTransformChangedFunction)
            onNodeTransformChangedFunction(changedNode);
    }

    void layer::nodeSelectionChanged(node* changedNode)
    {
        for (auto onNodeSelectionChangedFunction : _onNodeSelectionChanged)
            if (onNodeSelectionChangedFunction)
                onNodeSelectionChangedFunction(changedNode);
    }

    void layer::add(node* node)
    {
        _root->addChild(node);
    }

    void layer::initialize()
    {
        for (auto renderPass : _renderPasses)
            renderPass->initialize();
    }

    void layer::update()
    {
        for (auto& onUpdateFunction : _onUpdate)
            onUpdateFunction();

        for (auto& controller : _controllers)
            controller->update();

        updateFrameUniforms();
    }

    void layer::render()
    {
        _frameUniformsBuffer->bindBufferBase(0);

        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    void layer::onMouseDown(mouseEventArgs* e)
    {
        if (_currentController && _currentController->onMouseDown(e))
            return;

        for (auto& controller : _controllers)
            if (controller != _currentController && controller->onMouseDown(e))
                break;
    }

    void layer::onMouseUp(mouseEventArgs* e)
    {
        if (_currentController && _currentController->onMouseUp(e))
            return;

        for (auto& controller : _controllers)
            if (controller != _currentController && controller->onMouseUp(e))
                break;
    }

    void layer::onMouseClick(mouseEventArgs* e)
    {
        if (_currentController && _currentController->onMouseClick(e))
            return;

        for (auto& controller : _controllers)
            if (controller != _currentController && controller->onMouseClick(e))
                break;
    }

    void layer::onMouseDoubleClick(mouseEventArgs* e)
    {
        if (_currentController && _currentController->onMouseDoubleClick(e))
            return;

        for (auto& controller : _controllers)
            if (controller != _currentController && controller->onMouseDoubleClick(e))
                break;
    }

    void layer::onMouseMove(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            if (controller->onMouseMove(e))
                break;
    }

    void layer::onBeginMouseWheel(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onBeginMouseWheel(e);
    }

    void layer::onMouseWheel(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onMouseWheel(e);
    }

    void layer::onEndMouseWheel(mouseEventArgs* e)
    {
        for (auto& controller : _controllers)
            controller->onEndMouseWheel(e);
    }

    void layer::onKeyDown(keyboardEventArgs* e)
    {
        if (_currentController && _currentController->onKeyDown(e))
            return;

        for (auto& controller : _controllers)
            if (controller->onKeyDown(e))
                break;
    }

    void layer::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_ESCAPE && _currentController)
        {
            _currentController->cancel();
        }

        for (auto& controller : _controllers)
            if (controller->onKeyUp(e))
                break;
    }

    void layer::addMouseController(inputController * controller)
    {
        controller->getRequestControlEvent()->assign([&](inputController* controller) 
        {
            if (_controllersStack.size() > 0 && _controllersStack.top() == controller)
                return;

            std::cout << controller;
            debug(" control granted");

            _controllersStack.push(controller);
            _currentController = controller;
        });

        controller->getResignControlEvent()->assign([&](inputController* controller)
        {
            std::cout << controller;
            debug(" control lost");

            _controllersStack.pop();

            if (_controllersStack.size() > 0)
            {
                _currentController = _controllersStack.top();
            }
            else
                _currentController = nullptr;
        });

         _controllers.push_back(controller);
    }

    void layer::resize(const resolution& resolution)
    {
        _camera->setResolution(resolution);

        for (auto& renderPass : _renderPasses)
            renderPass->resize(resolution);
    }
}
