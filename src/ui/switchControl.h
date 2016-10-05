#pragma once

#include <phi.h>
#include <core/component.h>
#include <core/node.h>
#include <animation/translateAnimation.h>
#include "control.h"

namespace phi
{
    class switchControl :
        public component
    {
    public:
        static componentType getComponentType() { return componentType::SWITCH_CONTROL; }

    public:
        switchControl();
        switchControl(const switchControl& original);
        ~switchControl();

        UI_API component* clone() const override;

        UI_API void setHandle(node* value);
        UI_API void setOptionA(node* value);
        UI_API void setOptionB(node* value);
        UI_API void setOptionACallback(action<> value) { _optionACallback = value; }
        UI_API void setOptionBCallback(action<> value) { _optionBCallback = value; }

    private:
        void virtual onNodeChanged(node* previousValue) override;
        void switchToOptionA();
        void switchToOptionB();
        void adjustLayout();

    private:
        int _currentOption;
        node* _handle;
        node* _optionA;
        node* _optionB;
        eventToken _nodeTransformChangedEventToken;
        translateAnimation* _switchOptionAnimation;
        action<> _optionACallback;
        action<> _optionBCallback;
    };
}