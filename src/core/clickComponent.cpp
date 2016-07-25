#include <precompiled.h>
#include "clickComponent.h"

namespace phi
{
    clickComponent::clickComponent() :
        component(componentType::CLICK),
        _onClick(vector<std::function<void(node*)>>()),
        _pickId(0)
    {
    }

    clickComponent::clickComponent(const clickComponent& original) :
        component(componentType::CLICK),
        _onClick(original._onClick),
        _pickId(0)
    {
    }

    clickComponent::~clickComponent()
    {
    }

    void clickComponent::onClick() const
    {
        auto node = getNode();
        for (auto& clickFunction : _onClick)
        {
            clickFunction(node);
        }
    }

    vec3 clickComponent::getSelectionColor() const
    {
        auto r = static_cast<float>(_pickId & 255);
        auto g = static_cast<float>((_pickId >> 8) & 255);
        auto b = static_cast<float>((_pickId >> 16) & 255);

        return vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    }

    void clickComponent::addOnClick(std::function<void(node*)> onClick)
    {
        _onClick.push_back(onClick);
    }

    component* clickComponent::clone() const
    {
        auto click = static_cast<const clickComponent*>(this);
        return new clickComponent(*click);
    }
}