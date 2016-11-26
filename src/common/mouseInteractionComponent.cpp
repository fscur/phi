#include <precompiled.h>
#include "mouseInteractionComponent.h"

#include <animation/emptyMouseClickAnimatable.h>
#include <animation/emptyMouseHoverAnimatable.h>

namespace phi
{
    mouseInteractionComponent::mouseInteractionComponent() :
        component(componentType::CLICK),
        _onMouseDown(vector<std::function<void(node*)>>()),
        _onMouseUp(vector<std::function<void(node*)>>()),
        _pickId(0),
        _mouseClickAnimatable(new emptyMouseClickAnimatable()),
        _mouseHoverAnimatable(new emptyMouseHoverAnimatable()),
        _isMouseOver(false)
    {
    }

    mouseInteractionComponent::mouseInteractionComponent(const mouseInteractionComponent& original) :
        component(componentType::CLICK),
        _onMouseDown(original._onMouseDown),
        _onMouseUp(original._onMouseUp),
        _pickId(0),
        _mouseClickAnimatable(original._mouseClickAnimatable->clone()),
        _mouseHoverAnimatable(original._mouseHoverAnimatable->clone()),
        _isMouseOver(original._isMouseOver)
    {
    }

    mouseInteractionComponent::~mouseInteractionComponent()
    {
        safeDelete(_mouseClickAnimatable);
        safeDelete(_mouseHoverAnimatable);
    }

    void mouseInteractionComponent::onMouseDown() const
    {
        _mouseClickAnimatable->onMouseDown();

        auto node = getNode();
        for (auto& mouseDownFunction : _onMouseDown)
        {
            mouseDownFunction(node);
        }
    }

    void mouseInteractionComponent::onMouseUp() const
    {
        _mouseClickAnimatable->onMouseUp();

        auto node = getNode();
        for (auto& mouseUpFunction : _onMouseUp)
        {
            mouseUpFunction(node);
        }
    }

    void mouseInteractionComponent::onMouseMove(mouseEventArgs* e) const
    {
        auto node = getNode();
        for (auto& mouseMoveFunction : _onMouseMove)
        {
            mouseMoveFunction(e, node);
        }
    }

    void mouseInteractionComponent::onMouseEnter()
    {
        _isMouseOver = true;
        _mouseHoverAnimatable->onMouseEnter();
    }

    void mouseInteractionComponent::onMouseLeave()
    {
        _isMouseOver = false;
        _mouseHoverAnimatable->onMouseLeave();
    }

    vec3 mouseInteractionComponent::getSelectionColor() const
    {
        auto r = static_cast<float>(_pickId & 255);
        auto g = static_cast<float>((_pickId >> 8) & 255);
        auto b = static_cast<float>((_pickId >> 16) & 255);

        return vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    }

    bool mouseInteractionComponent::isMouseOver() const
    {
        return _isMouseOver;
    }

    void mouseInteractionComponent::addOnMouseDown(std::function<void(node*)> onMouseDown)
    {
        _onMouseDown.push_back(onMouseDown);
    }

    void mouseInteractionComponent::addOnMouseUp(std::function<void(node*)> onMouseUp)
    {
        _onMouseUp.push_back(onMouseUp);
    }

    void mouseInteractionComponent::addOnMouseMove(std::function<void(mouseEventArgs*, node*)> onMouseMove)
    {
        _onMouseMove.push_back(onMouseMove);
    }

    void mouseInteractionComponent::addOnMouseEnter(std::function<void(node*)> onHover)
    {
        _onMouseEnter.push_back(onHover);
    }

    void mouseInteractionComponent::addOnMouseLeave(std::function<void(node*)> onMouseLeave)
    {
        _onMouseLeave.push_back(onMouseLeave);
    }

    void mouseInteractionComponent::setMouseClickAnimatable(iMouseClickAnimatable* animatable)
    {
        _mouseClickAnimatable = animatable;
    }

    void mouseInteractionComponent::setMouseHoverAnimatable(iMouseHoverAnimatable* animatable)
    {
        _mouseHoverAnimatable = animatable;
    }

    component* mouseInteractionComponent::clone() const
    {
        auto click = static_cast<const mouseInteractionComponent*>(this);
        return new mouseInteractionComponent(*click);
    }
}