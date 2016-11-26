#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\font.h>
#include "control.h"

namespace phi
{
    class text :
        public component
    {
    public:
        static componentType getComponentType() { return componentType::TEXT; }

    public:
        text();
        text(const text& text);
        ~text();

        UI_API component* clone() const override;

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getColor() const { return _color; }
        control* getControl() const { return _control; }
        bool isBillboard() const { return _isBillboard; }

        UI_API void setText(wstring value);
        UI_API void setFont(font* value);
        UI_API void setIsBillboard(bool value) { _isBillboard = value; }

        void setColor(color value) { _color = value; }
        void setControl(control* value) { _control = value; }

    private:
        void updateControl();

    public:
        eventHandler<text*> textChangedEvent;

    private:
        font* _font;
        control* _control;
        wstring _text;
        color _color;
        bool _isBillboard;
    };
}