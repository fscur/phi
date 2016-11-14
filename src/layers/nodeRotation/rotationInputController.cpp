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
        _isEnabled(true),
        _isWaitingToStart(false),
        _startTimer(0.0f),
        _startMousePosition(ivec2()),
        _startClickedNode(nullptr)
    {
    }

    rotationInputController::~rotationInputController()
    {
        safeDelete(_rotationService);
    }

    bool rotationInputController::canStartRotation(mouseEventArgs* e, node*& clickedNode)
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
        if (node->isSelected())
        {
            clickedNode = node;
            return true;
        }

        return false;
    }

    void rotationInputController::startRotation()
    {
        _isWaitingToStart = false;

        for (auto& node : *_targetNodes)
        {
            _originalPositions[node] = node->getTransform()->getLocalPosition();
            _originalOrientations[node] = node->getTransform()->getLocalOrientation();
        }

        _rotationService->startRotation(_startMousePosition, _startClickedNode);
        rotationStarted.raise();

        _isMouseHidden = true;
        //window::hideCursor();

        _requestControlEvent->raise(this);
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
        node* clickedNode;
        if (!canStartRotation(e, clickedNode))
            return false;

        _startTimer = 0.5;
        _isWaitingToStart = true;
        _startMousePosition = ivec2(e->x, e->y);
        _startClickedNode = clickedNode;

        return false;
    }

    bool rotationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_rotationService->isRotating())
        {
            if (!_isWaitingToStart)
                return false;

            startRotation();
        }

        _rotationService->rotate(ivec2(e->x, e->y));
        return true;
    }

    bool rotationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        _isWaitingToStart = false;

        if (!_rotationService->isRotating())
            return false;

        _rotationService->endRotation();
        rotationEnded.raise();

        pushRotateCommands();

        _isMouseHidden = false;
        //window::showCursor();

        _resignControlEvent->raise(this);

        return true;
    }

    bool rotationInputController::onMouseClick(mouseEventArgs* e)
    {
        _unused(e);
        if (_rotationService->isRotating())
          return true;

        // TODO: Não sei se isso é gambis. Discutir!!

        return false;
    }

    bool rotationInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
            _rotationService->disableCollisions();

        if (e->key == PHIK_1)
            _rotationService->setUsageMode(rotationService::rotationUsageMode::ROTATE_AT_CENTROID);
        else if (e->key == PHIK_2)
            _rotationService->setUsageMode(rotationService::rotationUsageMode::ROTATE_AT_MOUSE_POSITION);
        else if (e->key == PHIK_3)
            _rotationService->setUsageMode(rotationService::rotationUsageMode::ROTATE_AT_INDIVIDUAL_ORIGINS);

        return false;
    }

    bool rotationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
            _rotationService->enableCollisions();

        return false;
    }

    bool rotationInputController::update()
    {
        if (_rotationService->isRotating())
        {
            _rotationService->update();
            return true;
        }
        
        if (!_isWaitingToStart)
            return false;

        _startTimer = glm::max(_startTimer - time::deltaSeconds, 0.0);
        if (_startTimer <= 0.0)
        {
            startRotation();
            return true;
        }

        return false;
    }

    void rotationInputController::cancel()
    {
        _rotationService->endRotation();
        rotationEnded.raise();

        for (auto& node : *_targetNodes)
            node->getTransform()->setLocalOrientation(_originalOrientations[node]);

        _resignControlEvent->raise(this);
    }
}