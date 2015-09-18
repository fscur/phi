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
        bool _clickedOver;
        color _backgroundColor;
        color _foregroundColor;
        color _overlayColor;
        texture* _texture;
        quadRenderer2D* _textureRenderer;
        quadRenderer2D* _overlayRenderer;
        textRenderer2D* _textRenderer;
        mouseEventHandler* _click;

    private:
        void updateTextLocation();
        void animateMouseEnter();
        void animateMouseLeave();
        void animatePressed();
        void animateUnpressed();

    public:
        UI_API button(size<GLuint> viewportSize);
        UI_API ~button();

        UI_API std::string getText() { return _text; }
        UI_API color getBackgroundColor() { return _backgroundColor; }
        UI_API color getForegroundColor() { return _foregroundColor; }
        UI_API font* getFont() { return _font; }
        UI_API mouseEventHandler* getClick() { return _click; }

        UI_API void setText(std::string value);
        UI_API void setBackgroundColor(color value);
        UI_API void setForegroundColor(color value) { _foregroundColor = value; }
        UI_API void setFont(font* value) { _font = value; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;

        UI_API void onMouseDown(mouseEventArgs* e) override;
        UI_API void onMouseUp(mouseEventArgs* e) override;
        UI_API void onMouseEnter(mouseEventArgs* e) override;
        UI_API void onMouseLeave(mouseEventArgs* e) override;

        UI_API void render() override;
    };
}

#endif