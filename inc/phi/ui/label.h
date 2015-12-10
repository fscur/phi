#ifndef _PHI_LABEL_H_
#define _PHI_LABEL_H_

#include "phi/core/globals.h"
#include "phi/rendering/quadRenderer2D.h"
#include "phi/rendering/textRenderer2D.h"
#include "phi/ui/control.h"

#include <string>

namespace phi
{
    class label :
		public control
    {
    private:
        std::string _text;
        font* _font;
        color _backgroundColor;
        color _foregroundColor;
        texture* _texture;
        quadRenderer2D* _backgroundRenderer;
        textRenderer2D* _textRenderer;
        int _textX;
        int _textY;

        void updateTextLocation();

    public:
		UI_API label(sizef viewportSize);
        UI_API ~label();

        UI_API std::string getText() { return _text; }
        UI_API font* getFont() { return _font; }
        UI_API color getBackgroundColor() { return _backgroundColor; }
        UI_API color getForegroundColor() { return _foregroundColor; }

        UI_API void setText(std::string value);
        UI_API void setFont(font* value) { _font = value; }
        UI_API void setBackgroundColor(color value) { _backgroundColor = value; }
        UI_API void setForegroundColor(color value) { _foregroundColor = value; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setSize(sizef value) override;
		UI_API void setViewportSize(sizef value) override;

        UI_API void onRender() override;
    };
}

#endif