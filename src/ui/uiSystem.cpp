#include <precompiled.h>
#include "uiSystem.h"

#include "floatAnimator.h"

namespace phi
{
    uiSystem* uiSystem::_instance = nullptr;

    uiSystem::uiSystem()
    {
        _controlGotFocus = new eventHandler<controlEventArgs>();
        _controlLostFocus = new eventHandler<controlEventArgs>();
        //_cursor = nullptr;

        input::mouseDown->assign(std::bind(&uiSystem::inputMouseDown, this, std::placeholders::_1));
        input::mouseUp->assign(std::bind(&uiSystem::inputMouseUp, this, std::placeholders::_1));
        input::mouseMove->assign(std::bind(&uiSystem::inputMouseMove, this, std::placeholders::_1));
        input::mouseWheel->assign(std::bind(&uiSystem::inputMouseWheel, this, std::placeholders::_1));
        input::keyDown->assign(std::bind(&uiSystem::inputKeyDown, this, std::placeholders::_1));
        input::keyDown->assign(std::bind(&uiSystem::inputKeyUp, this, std::placeholders::_1));
    }

    uiSystem::~uiSystem()
    {
        safeDelete(_gl);
        safeDelete(_font);
        safeDelete(_uiRenderer);
    }

    uiSystem* uiSystem::get()
    {
        if (_instance == nullptr)
            _instance = new uiSystem();

        return _instance;
    }

    void uiSystem::init(uiSystemInfo info)
    {
        _gl = info.gl;

        initRenderer(info.size.w, info.size.h);

        if (!uiRepository::initialized)
        {
            phi::uiRepositoryInfo repositoryInfo = phi::uiRepositoryInfo();
            repositoryInfo.applicationPath = info.applicationPath;
            repositoryInfo.resourcesPath = info.resourcesPath;
            repositoryInfo.size = info.size;
            phi::uiRepository::init(repositoryInfo);
        }

        input::mouseMove->assign(std::bind(&uiSystem::inputMouseMove, this, std::placeholders::_1));
    }

    void uiSystem::initRenderer(float width, float height)
    {
        _uiRenderer = new uiRenderer(_gl, width, height);
    }

    void uiSystem::setCursor(cursor* value)
    {
        /*vec2 mousePos;

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
        _cursorRenderer->setSize(sizeui(textureWitdh, textureHeight));
        _cursorRenderer->update();*/
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
        /*auto w = _cursor->getTexture()->w;
        auto h = _cursor->getTexture()->h;

        vec2 hotPoint = _cursor->getHotPoint();
        _cursorRenderer->setLocation(vec2(e->x - w * hotPoint.x, e->y - h * hotPoint.y));
        _cursorRenderer->update();*/

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

    void uiSystem::inputKeyDown(keyboardEventArgs* e)
    {
        for (control* control : _controls)
        {
            if (control->getIsFocused())
                control->notifyKeyDown(e);
        }
    }

    void uiSystem::inputKeyUp(keyboardEventArgs* e)
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
        _controlGotFocus->raise(e);
    }

    void uiSystem::notifyControlLostFocus(controlEventArgs e)
    {
        _controlLostFocus->raise(e);
    }

    void uiSystem::addControl(control* control)
    {
        _controlsToAdd.push_back(std::pair<phi::control*, bool>(control, true));
    }

    void uiSystem::removeControl(control* control)
    {
        _controlsToRemove.push_back(control);
    }

    void uiSystem::addControlToLists(control* control, bool root)
    {
        auto index = std::upper_bound(_controls.begin(), _controls.end(), control, [](phi::control* lhs, phi::control* rhs)
        {
            return lhs->getZIndex() > rhs->getZIndex();
        });

        controlEventTokens eventTokens;
        eventTokens.propertyChanged = control->getPropertyChanged()->assign(std::bind(&uiSystem::controlProperyChanged, this, std::placeholders::_1));
        eventTokens.gotFocus = control->getGotFocus()->assign(std::bind(&uiSystem::controlGotFocus, this, std::placeholders::_1));
        eventTokens.mouseLeave = control->getMouseLeave()->assign(std::bind(&uiSystem::controlMouseLeave, this, std::placeholders::_1));
        eventTokens.addedChild = control->getAddedChild()->assign(std::bind(&uiSystem::controlAddedChild, this, std::placeholders::_1));
        eventTokens.removedChild = control->getRemovedChild()->assign(std::bind(&uiSystem::controlRemovedChild, this, std::placeholders::_1));
        _controls.insert(index, control);
        _controlsEventTokens[control] = eventTokens;

        if (root)
            _rootControls.push_back(control);

        for (phi::control* child : control->getChildren())
            addControlToLists(child, false);
    }

    void uiSystem::removeControlFromLists(control* control)
    {
        auto eventTokens = _controlsEventTokens[control];
        control->getGotFocus()->unassign(eventTokens.gotFocus);
        control->getMouseLeave()->unassign(eventTokens.mouseLeave);
        control->getAddedChild()->unassign(eventTokens.addedChild);
        control->getRemovedChild()->unassign(eventTokens.removedChild);

        for (phi::control* child : control->getChildren())
            removeControlFromLists(child);

        _controls.erase(std::remove(_controls.begin(), _controls.end(), control), _controls.end());

        auto root = std::find(_rootControls.begin(), _rootControls.end(), control);
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

    void uiSystem::controlProperyChanged(control* sender)
    { 
        if (sender == nullptr)
            _uiRenderer->update(_controls);
        else
            //_uiRenderer->update(sender);
            _uiRenderer->update(_controls);
    }

    void uiSystem::resize(sizef size)
    {
        _info.size = size;

        /*for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->setViewportSize(size);
        }*/
    }

    void uiSystem::update()
    {
        bool dirty = false;

        for (std::pair<control*, bool> pair : _controlsToAdd)
        {
            addControlToLists(pair.first, pair.second);
            dirty = true;
        }

        _controlsToAdd.clear();

        for (auto control : _controlsToRemove)
        {
            removeControlFromLists(control);
            dirty = true;
        }

        _controlsToRemove.clear();

        floatAnimator::update();

        for (auto control : _controls)
            control->update();

        if (dirty)
            controlProperyChanged(nullptr);
    }

    void uiSystem::render()
    {
        _uiRenderer->render();
    }

    void uiSystem::renderCursor()
    {
        //_cursorRenderer->render(_cursor->getTexture(), color::white);
    }
}