#ifndef _PHI_TOGGLE_BUTTON_H_
#define _PHI_TOGGLE_BUTTON_H_

#include "phi/ui/button.h"
#include "phi/ui/controlCancelEventArgs.h"

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
        UI_API toggleButton(size<GLuint> viewportSize);

        UI_API bool getChecked() const { return _checked; }
        UI_API color getCheckedColor() const { return _checkedColor; }
        UI_API eventHandler<controlCancelEventArgs*>* getCheckedChanging() { return _checkedChanging; }
        UI_API eventHandler<controlEventArgs>* getCheckedChanged() { return _checkedChanged; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;
        UI_API void setChecked(const bool value);
        UI_API void setCheckedColor(const color value) { _checkedColor = value; }

        UI_API void onRender() override;
    };
}

#endif