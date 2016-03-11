#pragma once
#include <phi.h>

#include <rendering\quadRenderer2D.h>
#include <rendering\textRenderer2D.h>

#include "control.h"

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
		label(sizeui viewportSize);
        ~label();

        std::string getText() { return _text; }
        font* getFont() { return _font; }
        color getBackgroundColor() { return _backgroundColor; }
        color getForegroundColor() { return _foregroundColor; }

        void setText(std::string value);
        void setFont(font* value) { _font = value; }
        void setBackgroundColor(color value) { _backgroundColor = value; }
        void setForegroundColor(color value) { _foregroundColor = value; }

        void setX(int value) override;
        void setY(int value) override;
        void setSize(sizeui value) override;
		void setViewportSize(sizeui value) override;

        void onRender() override;
    };
}