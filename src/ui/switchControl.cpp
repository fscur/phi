#include <precompiled.h>

#include <core/node.h>
#include <common/mouseInteractionComponent.h>
#include <animation/animator.h>

#include "layoutTransform.h"
#include "switchControl.h"

namespace phi
{
    switchControl::switchControl() :
        component(componentType::SWITCH_CONTROL),
        _currentOption(0),
        _handle(nullptr),
        _optionA(nullptr),
        _optionB(nullptr)
    {
    }

    switchControl::switchControl(const switchControl& original) :
        component(componentType::SWITCH_CONTROL),
        _currentOption(original._currentOption),
        _handle(original._handle),
        _optionA(original._optionA),
        _optionB(original._optionB)
    {
        auto node = getNode();
        if (node)
            _nodeTransformChangedEventToken = node->getTransform()->getChangedEvent()->assign([&](transform* transform) { adjustLayout(); });
    }

    switchControl::~switchControl()
    {
        auto node = getNode();
        if (node)
            node->getTransform()->getChangedEvent()->unassign(_nodeTransformChangedEventToken);
    }

    void switchControl::onNodeChanged(node* previousValue)
    {
        if (previousValue)
            previousValue->getTransform()->getChangedEvent()->unassign(_nodeTransformChangedEventToken);

        auto node = getNode();
        if (node)
            _nodeTransformChangedEventToken = node->getTransform()->getChangedEvent()->assign([&](transform* transform) { adjustLayout(); });
    }

    component* switchControl::clone() const
    {
        return new switchControl(*this);
    }

    inline void switchControl::setHandle(node* value)
    {
        _handle = value;
        _switchOptionAnimation = new translateAnimation([&](vec3 position) { _handle->getTransform()->setLocalPosition(position); }, easingFunctions::easeOutQuint);

        auto animator = _handle->getComponent<phi::animator>();
        animator->addAnimation(_switchOptionAnimation);
        adjustLayout();
    }

    void switchControl::setOptionA(node* value)
    {
        _optionA = value;
        _optionA->getComponent<mouseInteractionComponent>()->addOnMouseUp([&](node* n)
        {
            switchToOptionA();
        });
        adjustLayout();
    }

    void switchControl::setOptionB(node* value)
    {
        _optionB = value;
        _optionB->getComponent<mouseInteractionComponent>()->addOnMouseUp([&](node* n)
        {
            switchToOptionB();
        });
        adjustLayout();
    }

    void switchControl::switchToOptionA()
    {
        if (_currentOption == 0)
            return;

        _currentOption = 0;
        adjustLayout();
    }

    void switchControl::switchToOptionB()
    {
        if (_currentOption == 1)
            return;

        _currentOption = 1;
        adjustLayout();
    }

    void switchControl::adjustLayout()
    {
        if (!_handle ||
            !_optionA ||
            !_optionB)
            return;

        auto optionALayout = static_cast<layoutTransform*>(_optionA->getTransform());
        auto optionBLayout = static_cast<layoutTransform*>(_optionB->getTransform());
        auto handleLayout = static_cast<layoutTransform*>(_handle->getTransform());
        auto switchLayout = static_cast<layoutTransform*>(getNode()->getTransform());
        auto switchSize = switchLayout->getSize();

        auto optionSize = vec3(switchSize.x, switchSize.y * 0.5f, 0.3f);

        optionALayout->setLocalSize(optionSize);
        optionALayout->setLocalPosition(vec3(0.0f, 0.0f, 1.0f));

        optionBLayout->setLocalSize(optionSize);
        optionBLayout->setLocalPosition(vec3(0.0f, -switchSize.y * 0.5f, 0.3f));

        handleLayout->setLocalSize(optionSize);
        if (_currentOption)
            _switchOptionAnimation->start(handleLayout->getLocalPosition(), vec3(0.0f, -switchSize.y * 0.5f, 0.2f), 0.2f);
        else
            _switchOptionAnimation->start(handleLayout->getLocalPosition(), vec3(0.0f, 0.0f, 0.2f), 0.2f);
    }
}