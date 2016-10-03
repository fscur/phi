#pragma once

#include <phi.h>
#include <core/component.h>
#include "control.h"

namespace phi
{
    class switchControl :
        public component
    {
    public:
        switchControl();
        switchControl(const switchControl& original);

        UI_API component* clone() const override;

        void setHandleControl(control* value) { _handleControl = value; adjustLayout(); }
        void setOptionAControl(control* value) { _optionAControl = value; adjustLayout(); }
        void setOptionBControl(control* value) { _optionBControl = value; adjustLayout(); }

    private:
        void virtual onNodeChanged(node* previousValue) override;
        void toggle();
        void adjustLayout();

    private:
        int _currentOption;
        control* _handleControl;
        control* _optionAControl;
        control* _optionBControl;
    };
}