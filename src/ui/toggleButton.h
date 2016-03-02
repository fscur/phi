#pragma once
#include <precompiled.h>

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
        UI_API toggleButton(sizef viewportSize);

        UI_API bool getChecked() const { return _checked; }
        UI_API color getCheckedColor() const { return _checkedColor; }
        UI_API eventHandler<controlCancelEventArgs*>* getCheckedChanging() { return _checkedChanging; }
        UI_API eventHandler<controlEventArgs>* getCheckedChanged() { return _checkedChanged; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(sizef value) override;
        UI_API void setViewportSize(sizef value) override;
        UI_API void setChecked(const bool value);
        UI_API void setCheckedColor(const color value) { _checkedColor = value; }

        UI_API void onRender() override;
    };
}