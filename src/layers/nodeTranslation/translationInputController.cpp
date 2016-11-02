#include <precompiled.h>

#include <core\multiCommand.h>

#include <animation\animator.h>

#include <rendering\pickingFramebuffer.h>

#include <context\pickingId.h>

#include "translationInputController.h"
#include "translateNodeCommand.h"
#include <application/window.h>

namespace phi
{
    translationInputController::translationInputController(
        commandsManager* commandsManager,
        const vector<node*>* targetNodes,
        layer* layer,
        physicsWorld* physicsWorld) :
        inputController(),
        _commandsManager(commandsManager),
        _targetNodes(targetNodes),
        _translationService(new translationService(targetNodes, layer, physicsWorld)),
        _isMouseHidden(false),
        _isEnabled(true),
        _isWaitingToStart(false),
        _startTimer(0.0f),
        _startMousePosition(ivec2())
    {
    }

    translationInputController::~translationInputController()
    {
        safeDelete(_translationService);
    }

    bool translationInputController::canStartTranslation(mouseEventArgs* e, node*& clickedNode)
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

    void translationInputController::pushTranslateCommands()
    {
        auto commands = vector<command*>();

        for (auto& node : *_targetNodes)
        {
            auto targetPosition = node->getTransform()->getLocalPosition();
            auto translateCommand = new translateNodeCommand(node, _originalPositions[node], targetPosition);
            commands.push_back(translateCommand);
        }

        auto multiCommand = new phi::multiCommand(commands);
        _commandsManager->pushCommand(multiCommand);
    }

    bool translationInputController::onMouseDown(mouseEventArgs* e)
    {
        node* clickedNode;
        if (!canStartTranslation(e, clickedNode))
            return false;

        _startTimer = 0.5;
        _isWaitingToStart = true;
        _startMousePosition = ivec2(e->x, e->y);
        _startClickedNode = clickedNode;
        return false;
    }

    void translationInputController::startTranslation()
    {
        for (auto& node : *_targetNodes)
            _originalPositions[node] = node->getTransform()->getLocalPosition();

        _translationService->startTranslation(_startMousePosition, _startClickedNode);

        _isMouseHidden = true;
        window::hideCursor();

        _requestControlEvent->raise(this);
        translationStarted.raise();
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_translationService->isTranslating())
        {
            if (!_isWaitingToStart)
                return false;

            startTranslation();
        }

        _translationService->translate(ivec2(e->x, e->y));
        return true;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        _isWaitingToStart = false;

        if (!_translationService->isTranslating())
            return false;

        _translationService->endTranslation();
        pushTranslateCommands();
        _isMouseHidden = false;
        window::showCursor();

        translationEnded.raise();
        _resignControlEvent->raise(this);

        return true;
    }

    bool translationInputController::onMouseClick(mouseEventArgs* e)
    {
        if (_translationService->isTranslating())
            return true;

        // TODO: Não sei se isso é gambis. Discutir!!

        return false;
    }

    bool translationInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (!_translationService->isTranslating())
            return false;

        if (e->key == PHIK_CTRL)
        {
            _translationService->disableCollisions();
            _translationService->disablePlaneChanges();
            return true;
        }

        if (e->key == PHIK_SHIFT)
        {
            _translationService->enableSnapToGrid();
            return true;
        }

        return false;
    }

    bool translationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (!_translationService->isTranslating())
            return false;

        if (e->key == PHIK_CTRL)
        {
            _translationService->enableCollisions();
            _translationService->enablePlaneChanges();
            return true;
        }

        if (e->key == PHIK_SHIFT)
        {
            _translationService->disableSnapToGrid();
            return true;
        }

        return false;
    }

    bool translationInputController::update()
    {
        if (_translationService->isTranslating())
        {
            _translationService->update();
            return true;
        }

        if (!_isWaitingToStart)
            return false;

        _startTimer = glm::max(_startTimer - time::deltaSeconds, 0.0);
        if (_startTimer <= 0.0)
        {
            startTranslation();
            _isWaitingToStart = false;
            return true;
        }

        return false;
    }

    void translationInputController::cancel()
    {
        _translationService->endTranslation();

        for (auto& node : *_targetNodes)
            node->getTransform()->setLocalPosition(_originalPositions[node]);

        _isMouseHidden = false;
        window::showCursor();

        translationEnded.raise();
        _resignControlEvent->raise(this);
    }
}