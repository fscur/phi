#ifndef _PHI_LABEL_H_
#define _PHI_LABEL_H_

#include <string>
#include "globals.h"
#include "quadRenderer2D.h"
#include "textRenderer2D.h"
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
        textRenderer2D* _textRenderer;
        int _textX;
        int _textY;

        void updateTextLocation();

    public:
		UI_API label(size<GLuint> viewportSize);
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
        UI_API void setSize(size<GLuint> value) override;
		UI_API void setViewportSize(size<GLuint> value) override;

        UI_API void onRender() override;
    };
}

#endif