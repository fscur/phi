#include "uiSystem.h"
#include "renderingSystem.h"

namespace phi
{
    uiSystem* uiSystem::_instance = nullptr;

    uiSystem::uiSystem()
    {
        _controlGotFocus = new eventHandler<controlEventArgs>();
        _controlLostFocus = new eventHandler<controlEventArgs>();
        _cursor = nullptr;
        _cursorRenderer = new quadRenderer2D(glm::vec2(0.0f, 0.0f), 1.0f, size<GLuint>(0.0f, 0.0f), size<GLuint>(0.0f, 0.0f));
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
            repositoryInfo.size = info.size;
            phi::uiRepository::init(repositoryInfo);
        }

        if (!renderingSystem::initialized)
        {
            renderingSystemInfo renderingInfo = renderingSystemInfo();
            renderingInfo.applicationPath = info.applicationPath;
            renderingInfo.size = info.size;
            renderingSystem::init(renderingInfo);
        }

        input::mouseMove->bind<uiSystem, &uiSystem::inputMouseMove>(this);
        setCursor(phi::uiRepository::repository->getResource<cursor>("DefaultCursor"));
        _cursorRenderer->setViewportSize(_info.size);
    }

    void uiSystem::setCursor(cursor* value)
    {
        glm::vec2 mousePos;
        if (_cursor != nullptr)
            mousePos = _cursorRenderer->getLocation() + glm::vec2(_cursor->getTexture()->getSize().width * _cursor->getHotPoint().x, _cursor->getTexture()->getSize().height * _cursor->getHotPoint().y);
        else
            mousePos = glm::vec2();

        _cursor = value;
        size<GLuint> size = _cursor->getTexture()->getSize();
        glm::vec2 hotPoint = _cursor->getHotPoint();
        _cursorRenderer->setLocation(glm::vec2(mousePos.x - size.width * hotPoint.x, mousePos.y - size.height * hotPoint.y));
        _cursorRenderer->setSize(_cursor->getTexture()->getSize());
        _cursorRenderer->update();
    }

    void uiSystem::inputMouseMove(mouseEventArgs e)
    {
        size<GLuint> size = _cursor->getTexture()->getSize();
        glm::vec2 hotPoint = _cursor->getHotPoint();
        _cursorRenderer->setLocation(glm::vec2(e.x - size.width * hotPoint.x, e.y - size.height * hotPoint.y));
        _cursorRenderer->update();
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

    void uiSystem::addControl(control* control)
    {
        _controls.push_back(control);
        control->setZIndex((_controls.size() - 1) * 0.1f);
        control->getGotFocus()->bind<uiSystem, &uiSystem::controlGotFocus>(this);
        control->getMouseLeave()->bind<uiSystem, &uiSystem::controlMouseLeave>(this);
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

    void uiSystem::controlMouseLeave(mouseEventArgs e)
    {
        setCursor(phi::uiRepository::repository->getResource<cursor>("DefaultCursor"));
    }

    void uiSystem::resize(size<GLuint> size)
    {
        _info.size = size;

        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->setViewportSize(size);
        }

        _cursorRenderer->setViewportSize(_info.size);
    }

    void uiSystem::render()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->render();
        }

        glDisable(GL_BLEND);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderCursor();

        glDisable(GL_BLEND);
    }

    void uiSystem::renderCursor()
    {
        _cursorRenderer->render(_cursor->getTexture(), color::white);
    }
}