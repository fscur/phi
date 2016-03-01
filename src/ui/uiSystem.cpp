#include "uiSystem.h"

#include <rendering\renderingSystem.h>

#include "floatAnimator.h"

#include <algorithm>

namespace phi
{
    uiSystem* uiSystem::_instance = nullptr;

    uiSystem::uiSystem()
    {
        _controlGotFocus = new eventHandler<controlEventArgs>();
        _controlLostFocus = new eventHandler<controlEventArgs>();
        _cursor = nullptr;

        input::mouseDown->bind<uiSystem, &uiSystem::inputMouseDown>(this);
        input::mouseUp->bind<uiSystem, &uiSystem::inputMouseUp>(this);
        input::mouseMove->bind<uiSystem, &uiSystem::inputMouseMove>(this);
        input::mouseWheel->bind<uiSystem, &uiSystem::inputMouseWheel>(this);
        input::keyDown->bind<uiSystem, &uiSystem::inputKeyDown>(this);
        input::keyDown->bind<uiSystem, &uiSystem::inputKeyUp>(this);
    }

    uiSystem* uiSystem::get()
    {
        if (_instance == nullptr)
            _instance = new uiSystem();

        return _instance;
    }

    void uiSystem::init(uiSystemInfo info)
    {
        if (!uiRepository::initialized)
        {
            phi::uiRepositoryInfo repositoryInfo = phi::uiRepositoryInfo();
            repositoryInfo.applicationPath = info.applicationPath;
            repositoryInfo.resourcesPath = info.resourcesPath;
            repositoryInfo.size = info.size;
            phi::uiRepository::init(repositoryInfo);
        }

        control::init(info.size);
        input::mouseMove->bind<uiSystem, &uiSystem::inputMouseMove>(this);
        _cursorRenderer = new quadRenderer2D(vec2(), 1.0f, sizef(), sizef());
        setCursor(phi::uiRepository::cursorDefault);
        _cursorRenderer->setViewportSize(info.size);
        dragDropController::get()->init(info.size);
        dragDropController::get()->getDradDropEnded()->bind<uiSystem, &uiSystem::dragDropEnded>(this);
    }

    void uiSystem::setCursor(cursor* value)
    {
        vec2 mousePos;

        if (_cursor != nullptr)
            mousePos = _cursorRenderer->getLocation() + vec2(_cursor->getTexture()->w * _cursor->getHotPoint().x, _cursor->getTexture()->h * _cursor->getHotPoint().y);
        else
            mousePos = vec2();

        _cursor = value;

        auto textureWitdh = _cursor->getTexture()->w;
        auto textureHeight = _cursor->getTexture()->h;

        vec2 hotPoint = _cursor->getHotPoint();
        _cursorRenderer->setLocation(vec2(mousePos.x - textureWitdh * hotPoint.x, mousePos.y - textureHeight * hotPoint.y));
        _cursorRenderer->setZIndex(50.0f);
        _cursorRenderer->setSize(sizef((float)textureWitdh, (float)textureHeight));
        _cursorRenderer->update();
    }

    void uiSystem::inputMouseDown(mouseEventArgs* e)
    {
        for (control* control : _controls)
        {
            if (control->getIsMouseOver())
                control->setIsFocused(true);

            control->notifyMouseDown(e);

            if (e->handled)
                return;
        }
    }

    void uiSystem::inputMouseUp(mouseEventArgs* e)
    {
        for (control* control : _controls)
        {
            control->notifyMouseUp(e);
        }
    }

    void uiSystem::inputMouseMove(mouseEventArgs* e)
    {
        auto w = _cursor->getTexture()->w;
        auto h = _cursor->getTexture()->h;

        vec2 hotPoint = _cursor->getHotPoint();
        _cursorRenderer->setLocation(vec2(e->x - w * hotPoint.x, e->y - h * hotPoint.y));
        _cursorRenderer->update();

        //control* higher = nullptr;
        //for (control* control : _controls)
        //{
        //    control->notifyMouseMove(e);

        //    if (!higher && control->isPointInside(e->x, e->y))
        //        higher = control;
        //    else
        //    {
        //        if (control->getIsMouseOver())
        //            control->notifyMouseLeave(e);
        //    }

        //    if (e->handled)
        //        break;
        //}

        //if (higher && !higher->getIsMouseOver())
        //    higher->notifyMouseEnter(e);

        auto topMost = true;
        for (control* control : _controls)
        {
            if (control->isPointInside(e->x, e->y))
            {
                control->setIsTopMost(topMost);
                topMost = false;

                if (!control->getIsMouseOver())
                    control->notifyMouseEnter(e);
            }
            else
            {
                if (control->getIsMouseOver())
                    control->notifyMouseLeave(e);
            }

            control->notifyMouseMove(e);
        }
    }

    void uiSystem::inputMouseWheel(mouseEventArgs* e)
    {
        for (control* control : _controls)
        {
            control->notifyMouseWheel(e);
        }
    }

    void uiSystem::inputKeyDown(keyboardEventArgs e)
    {
        for (control* control : _controls)
        {
            if (control->getIsFocused())
                control->notifyKeyDown(e);
        }
    }

    void uiSystem::inputKeyUp(keyboardEventArgs e)
    {
        for (control* control : _controls)
        {
            if (control->getIsFocused())
                control->notifyKeyUp(e);
        }
    }

    void uiSystem::dragDropEnded(dragDropEventArgs* e)
    {
        // Implementar para que o drag drop ended seja notificado para todos os controles
    }

    void uiSystem::notifyControlGotFocus(controlEventArgs e)
    {
        if (_controlGotFocus->isBound())
            _controlGotFocus->invoke(e);
    }

    void uiSystem::notifyControlLostFocus(controlEventArgs e)
    {
        if (_controlLostFocus->isBound())
            _controlLostFocus->invoke(e);
    }

    void uiSystem::addControl(control* cntrl)
    {
        _controlsToAdd.push_back(std::pair<control*, bool>(cntrl, true));
    }

    void uiSystem::removeControl(control* cntrl)
    {
        _controlsToRemove.push_back(cntrl);
    }

    void uiSystem::addControlToList(control* cntrl, bool root)
    {
        auto index = std::upper_bound(_controls.begin(), _controls.end(), cntrl, [](control* lhs, control* rhs)
        {
            return lhs->getZIndex() > rhs->getZIndex();
        });

        auto zIndex = cntrl->getZIndex();
        cntrl->getGotFocus()->bind<uiSystem, &uiSystem::controlGotFocus>(this);
        cntrl->getMouseLeave()->bind<uiSystem, &uiSystem::controlMouseLeave>(this);
        cntrl->getAddedChild()->bind<uiSystem, &uiSystem::controlAddedChild>(this);
        cntrl->getRemovedChild()->bind<uiSystem, &uiSystem::controlRemovedChild>(this);
        _controls.insert(index, cntrl);

        if (root)
            _rootControls.push_back(cntrl);

        for (control* child : cntrl->getChildren())
            addControlToList(child, false);
    }

    void uiSystem::removeControlFromList(control* cntrl)
    {
        cntrl->getGotFocus()->unbind<uiSystem, &uiSystem::controlGotFocus>(this);
        cntrl->getMouseLeave()->unbind<uiSystem, &uiSystem::controlMouseLeave>(this);
        cntrl->getAddedChild()->unbind<uiSystem, &uiSystem::controlAddedChild>(this);
        cntrl->getRemovedChild()->unbind<uiSystem, &uiSystem::controlRemovedChild>(this);

        for (control* child : cntrl->getChildren())
            removeControlFromList(child);

        _controls.erase(std::remove(_controls.begin(), _controls.end(), cntrl), _controls.end());

        auto root = std::find(_rootControls.begin(), _rootControls.end(), cntrl);
        if (root != _rootControls.end())
            _rootControls.erase(root);
    }

    void uiSystem::controlGotFocus(controlEventArgs e)
    {
        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            if (_controls[i] != e.sender)
            {
                if (_controls[i]->getIsFocused())
                {
                    _controls[i]->setIsFocused(false);
                    notifyControlLostFocus(controlEventArgs(_controls[i]));
                }
            }
        }

        notifyControlGotFocus(controlEventArgs(e.sender));
    }

    void uiSystem::controlMouseLeave(mouseEventArgs* e)
    {
        //setCursor(phi::uiRepository::cursorsRepository->getResource("DefaultCursor"));
    }

    void uiSystem::controlAddedChild(controlEventArgs e)
    {
        _controlsToAdd.push_back(std::pair<control*, bool>((control*)e.sender, false));
    }

    void uiSystem::controlRemovedChild(controlEventArgs e)
    {
        removeControl((control*)e.sender);
    }

    void uiSystem::resize(sizef size)
    {
        _info.size = size;

        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->setViewportSize(size);
        }

        _cursorRenderer->setViewportSize(_info.size);
        _cursorRenderer->update();
    }

    void uiSystem::update()
    {
        for (std::pair<control*, bool> pair : _controlsToAdd)
            addControlToList(pair.first, pair.second);
        _controlsToAdd.clear();

        for (control* cntrl : _controlsToRemove)
            removeControlFromList(cntrl);
        _controlsToRemove.clear();

        floatAnimator::update();

        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->update();
        }
    }

    void uiSystem::render()
    {
        glDepthMask(GL_TRUE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (control* ctrl : _rootControls)
            ctrl->render();

        dragDropController::get()->render();

        glDisable(GL_BLEND);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderCursor();

        glDisable(GL_BLEND);
    }

    void uiSystem::renderCursor()
    {
        //_cursorRenderer->render(_cursor->getTexture(), color::white);
    }
}