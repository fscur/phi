#pragma once
#include <phi.h>

#include "button.h"
#include "controlCancelEventArgs.h"

namespace phi
{
    class toggleButton : public button
    {
    private:
        bool _checked;
        color _checkedColor;
        color _checkedCurrentColor;
        eventHandler<controlCancelEventArgs*>* _checkedChanging;
        eventHandler<controlEventArgs>* _checkedChanged;
        quadRenderer2D* _checkedBackgroundRenderer;

    protected:
        void onClick() override;

    public:
        toggleButton(sizef viewportSize);

        bool getChecked() const { return _checked; }
        color getCheckedColor() const { return _checkedColor; }
        eventHandler<controlCancelEventArgs*>* getCheckedChanging() { return _checkedChanging; }
        eventHandler<controlEventArgs>* getCheckedChanged() { return _checkedChanged; }

        void setX(int value) override;
        void setY(int value) override;
        void setZIndex(float value) override;
        void setSize(sizef value) override;
        void setViewportSize(sizef value) override;
        void setChecked(const bool value);
        void setCheckedColor(const color value) { _checkedColor = value; }

        void onRender() override;
    };
}