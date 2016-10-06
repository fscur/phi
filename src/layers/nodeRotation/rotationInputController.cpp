#include <precompiled.h>

#include <core/multiCommand.h>

#include <animation/animator.h>

#include <rendering/pickingFramebuffer.h>

#include <context/pickingId.h>

#include <application/window.h>

#include "rotationInputController.h"
#include "rotateNodeCommand.h"
#include "../nodeTranslation/translateNodeCommand.h"

namespace phi
{
    rotationInputController::rotationInputController(
        commandsManager* commandsManager,
        const vector<node*>* targetNodes,
        layer* layer,
        physicsWorld* physicsWorld) :
        inputController(),
        _commandsManager(commandsManager),
        _targetNodes(targetNodes),
        _rotationService(new rotationService(targetNodes, layer, physicsWorld)),
        _isMouseHidden(false),
        _isEnabled(true)
    {
    }

    rotationInputController::~rotationInputController()
    {
        safeDelete(_rotationService);
    }

    bool rotationInputController::canStartRotation(mouseEventArgs* e)
    {
        if (!_isEnabled)
            return false;

        if (!e->leftButtonPressed)
            return false;

        if (_targetNodes->size() == 0)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (!clickComponent)
            return false;

        auto node = clickComponent->getNode();
        auto rootNode = node;

        if (rootNode->isSelected())
            return true;

        return false;
    }

    void rotationInputController::pushRotateCommands()
    {
        auto commands = vector<command*>();

        for (auto& node : *_targetNodes)
        {
            auto targetPosition = node->getTransform()->getLocalPosition();
            auto targetOrientation = node->getTransform()->getLocalOrientation();
            auto rotateCommand = new rotateNodeCommand(node, _originalOrientations[node], targetOrientation);
            auto translateCommand = new translateNodeCommand(node, _originalPositions[node], targetPosition);
            commands.push_back(new multiCommand({ rotateCommand, translateCommand }));
        }

        auto multiCommand = new phi::multiCommand(commands);
        _commandsManager->pushCommand(multiCommand);
    }

    bool rotationInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!canStartRotation(e))
            return false;

        for (auto& node : *_targetNodes)
        {
            _originalPositions[node] = node->getTransform()->getLocalPosition();
            _originalOrientations[node] = node->getTransform()->getLocalOrientation();
        }

        _rotationService->startRotation(ivec2(e->x, e->y));

        _isMouseHidden = true;
        //window::hideCursor();

        _requestControlEvent->raise(this);
        rotationStarted.raise();

        return false;
    }

    bool rotationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_rotationService->isRotating())
            return false;

        _rotationService->rotate(ivec2(e->x, e->y));
        return true;
    }

    bool rotationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_rotationService->isRotating())
            return false;

        _rotationService->endRotation();

        pushRotateCommands();

        _isMouseHidden = false;
        //window::showCursor();

        _resignControlEvent->raise(this);
        rotationEnded.raise();

        return true;
    }

    bool rotationInputController::onMouseClick(mouseEventArgs* e)
    {
        if (_rotationService->isRotating())
          return true;

        // TODO: Não sei se isso é gambis. Discutir!!

        return false;
    }

    bool rotationInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL && _rotationService->isRotating())
        {
            _rotationService->disableCollisions();
            
            return true;
        }

        return false;
    }

    bool rotationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL && _rotationService->isRotating())
        {
            _rotationService->enableCollisions();

            return true;
        }

        return false;
    }

    bool rotationInputController::update()
    {
        _rotationService->update();
        
        return false;
    }

    void rotationInputController::cancel()
    {
        _rotationService->endRotation();

        for (auto& node : *_targetNodes)
            node->getTransform()->setLocalOrientation(_originalOrientations[node]);

        _resignControlEvent->raise(this);
    }
}