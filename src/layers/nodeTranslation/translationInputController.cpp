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
        _commandsManager(commandsManager),
        inputController(),
        _translationService(new translationService(targetNodes, layer, physicsWorld)),
        _targetNodes(targetNodes),
        _isMouseHidden(false)
    {
    }

    translationInputController::~translationInputController()
    {
        safeDelete(_translationService);
    }

    bool translationInputController::canStartTranslation(mouseEventArgs* e)
    {
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
        if (!canStartTranslation(e))
            return false;

        for (auto& node : *_targetNodes)
            _originalPositions[node] = node->getTransform()->getLocalPosition();

        _translationService->startTranslation(ivec2(e->x, e->y));

        _requestControlEvent->raise(this);

        _isMouseHidden = true;
        //window::hideCursor();

        return false;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        _translationService->translate(ivec2(e->x, e->y));
        return true;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_translationService->isTranslating())
            return false;

        _translationService->endTranslation();

        pushTranslateCommands();

        _resignControlEvent->raise(this);

        _isMouseHidden = false;
        //window::showCursor();

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
        if (e->key == PHIK_CTRL && _translationService->isTranslating())
        {
            _translationService->disableCollisions();
            _translationService->disablePlaneChanges();
            return true;
        }

        return false;
    }

    bool translationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL && _translationService->isTranslating())
        {
            _translationService->enableCollisions();
            _translationService->enablePlaneChanges();
            return true;
        }

        return false;
    }

    bool translationInputController::update()
    {
        _translationService->update();

        /*if (!_translationService->isTranslating() && _isMouseHidden)
        {
            _isMouseHidden = false;
            window::showCursor();
        }
        else if (_translationService->isTranslating() && !_isMouseHidden)
        {
            _isMouseHidden = true;
            window::hideCursor();
        }*/
        return false;
    }

    void translationInputController::cancel()
    {
        _translationService->endTranslation();

        for (auto& node : *_targetNodes)
            node->getTransform()->setLocalPosition(_originalPositions[node]);

        _resignControlEvent->raise(this);
    }
}