#pragma once
#include <phi.h>
#include "control.h"

namespace phi
{
    class label :
		public control
    {
    private:
        font* _font;
        wstring _text;

        void updateTextLocation();

    public:
		UI_API label();
        UI_API ~label();

        wstring getText() const { return _text; }
        font* getFont() const { return _font; }

        UI_API void setPosition(vec3 value) override;
        UI_API void setSize(sizef value) override;
        UI_API void setText(wstring value);
        UI_API void setFont(font* value);
    };
}