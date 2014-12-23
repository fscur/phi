#ifndef _PHI_BUTTON_H_
#define _PHI_BUTTON_H_

#include <string>
#include "quadRenderer2D.h"
#include "textRenderer2D.h"
#include "control.h"
#include "globals.h"

namespace phi
{
    class button : public control
    {
    private:
        std::string _text;
        font* _font;
        int _textX;
        int _textY;
        color _backgroundColor;
        color _foregroundColor;
        color _currentColor;
        texture* _texture;
        quadRenderer2D* _textureRenderer;
        textRenderer2D* _textRenderer;
        bool _clickedOver;
        mouseEventHandler* _click;

        void updateTextLocation();

    public:
		UI_API button(size<GLuint> viewportSize);
        UI_API ~button();

        UI_API std::string getText() { return _text; }
        UI_API color getBackgroundColor() { return _backgroundColor; }
        UI_API color getForegroundColor() { return _foregroundColor; }
        UI_API mouseEventHandler* getClick() { return _click; }

        UI_API void setText(std::string value);
        UI_API void setBackgroundColor(color value);
        UI_API void setForegroundColor(color value) { _foregroundColor = value; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setSize(size<GLuint> value) override;
		UI_API void setViewportSize(size<GLuint> value) override;

        UI_API void onMouseDown(mouseEventArgs e) override;
        UI_API void onMouseUp(mouseEventArgs e) override;
        UI_API void onMouseEnter(mouseEventArgs e) override;
        UI_API void onMouseLeave(mouseEventArgs e) override;

        UI_API void render() override;
    };
}

#endif