#include <precompiled.h>

#include <core/node.h>
#include <common/mouseInteractionComponent.h>

#include "layoutTransform.h"
#include "switchControl.h"

namespace phi
{
    switchControl::switchControl() :
        component(componentType::SWITCH_CONTROL),
        _currentOption(0),
        _handleControl(nullptr),
        _optionAControl(nullptr),
        _optionBControl(nullptr)
    {
    }

    switchControl::switchControl(const switchControl& original) :
        component(componentType::SWITCH_CONTROL),
        _currentOption(original._currentOption),
        _handleControl(original._handleControl),
        _optionAControl(original._optionAControl),
        _optionBControl(original._optionBControl)
    {
        getNode()->getTransform()->getChangedEvent()->assign([&](transform* transform) { adjustLayout(); });
    }

    component* switchControl::clone() const
    {
        auto c = static_cast<const switchControl*>(this);
        return new switchControl(*c);
    }

    void switchControl::onNodeChanged(node * previousValue)
    {
        getNode()->getTransform()->getChangedEvent()->assign([&](transform* transform) { adjustLayout(); });
        getNode()->getComponent<mouseInteractionComponent>()->addOnMouseUp([&](node* n)
        {
            toggle();
        });
    }

    void switchControl::toggle()
    {
        _currentOption = !_currentOption;
        adjustLayout();
    }

    void switchControl::adjustLayout()
    {
        if (!_handleControl ||
            !_optionAControl ||
            !_optionBControl)
            return;

        auto optionALayout = static_cast<layoutTransform*>(_optionAControl->getNode()->getTransform());
        auto optionBLayout = static_cast<layoutTransform*>(_optionBControl->getNode()->getTransform());
        auto handleLayout = static_cast<layoutTransform*>(_handleControl->getNode()->getTransform());
        auto switchLayout = static_cast<layoutTransform*>(getNode()->getTransform());
        auto switchSize = switchLayout->getSize();

        auto optionSize = vec3(switchSize.x, switchSize.y * 0.5f, 0.2f);

        optionALayout->setLocalSize(optionSize);
        optionALayout->setLocalPosition(vec3(0.0f, 0.0f, 1.0f));

        optionBLayout->setLocalSize(optionSize);
        optionBLayout->setLocalPosition(vec3(0.0f, -switchSize.y * 0.5f, 0.2f));

        handleLayout->setLocalSize(optionSize);
        if (_currentOption)
            handleLayout->setLocalPosition(vec3(0.0f, 0.0f, 0.3f));
        else
            handleLayout->setLocalPosition(vec3(0.0f, -switchSize.y * 0.5f, 0.3f));
    }
}