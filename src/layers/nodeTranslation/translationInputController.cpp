#include <precompiled.h>

#include "translationInputController.h"

#include <animation\animator.h>
#include <rendering\pickingFramebuffer.h>

#include <context\pickingId.h>

namespace phi
{
    translationInputController::translationInputController(
        const vector<node*>* targetNodes, 
        layer* layer, 
        physicsWorld* physicsWorld) :
        inputController(),
        _translationService(new translationService(targetNodes, layer, physicsWorld))
    {
    }

    translationInputController::~translationInputController()
    {
    }

    bool translationInputController::canStartTranslation(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        if (_translationService->getTargetNodesCount() == 0)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (!clickComponent)
            return false;

        auto node = clickComponent->getNode();
        auto rootNode = node;

        while (rootNode->getParent()->getParent() != nullptr)
        {
            rootNode = rootNode->getParent();
            if (rootNode->isSelected())
                return true;
        }

        return false;
    }

    bool translationInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!canStartTranslation(e))
            return false;

        _translationService->startTranslation(ivec2(e->x, e->y));

        _requestControlEvent->raise(this);

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

        return false;
    }

    void translationInputController::cancel()
    {
        _translationService->cancelTranslation();
        _translationService->endTranslation();
        _resignControlEvent->raise(this);
    }
}