#ifndef _PHI_BUTTON_H_
#define _PHI_BUTTON_H_

#include "phi/core/globals.h"

#include "phi/rendering/quadRenderer2D.h"
#include "phi/rendering/textRenderer2D.h"

#include "phi/ui/control.h"

#include <string>

namespace phi
{
    class button : public control
    {
    private:
        std::string _text;
        texture* _image;
        font* _font;
        int _textX;
        int _textY;
        bool _clickedOver;
        color _foregroundColor;
        color _overlayColor;
        quadRenderer2D* _backgroundRenderer;
        quadRenderer2D* _overlayRenderer;
        textRenderer2D* _textRenderer;
        quadRenderer2D* _imageRenderer;
        mouseEventHandler* _click;

    protected:
        color _backgroundColor;
        color _currentBackgroundColor;
        texture* _backgroundTexture;

    private:
        void updateTextLocation();
        void updateImageSize();
        void updateImageLocation();

    protected:
        void virtual onClick() {}
        virtual void animateMouseEnter();
        virtual void animateMouseLeave();
        virtual void animatePressed();
        virtual void animateUnpressed();
        virtual void renderBackground();
        virtual void renderOverlay();
        virtual void renderImage();
        virtual void renderForeground();

    public:
        UI_API button(sizef viewportSize);
        UI_API ~button();

        UI_API std::string getText() { return _text; }
        UI_API texture* getTexture() { return _image; }
        UI_API color getBackgroundColor() { return _backgroundColor; }
        UI_API color getForegroundColor() { return _foregroundColor; }
        UI_API font* getFont() { return _font; }
        UI_API mouseEventHandler* getClick() { return _click; }

        UI_API void setText(std::string value);
        UI_API void setImage(texture* value);
        UI_API void setBackgroundColor(color value);
        UI_API void setForegroundColor(color value) { _foregroundColor = value; }
        UI_API void setFont(font* value) { _font = value; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(sizef value) override;
        UI_API void setViewportSize(sizef value) override;
        UI_API void setIsTopMost(bool value) override;

        UI_API void onMouseDown(mouseEventArgs* e) override;
        UI_API void onMouseUp(mouseEventArgs* e) override;
        UI_API void onMouseEnter(mouseEventArgs* e) override;
        UI_API void onMouseLeave(mouseEventArgs* e) override;

        UI_API void onRender() override;
    };
}

#endif