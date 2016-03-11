#pragma once
#include <phi.h>

#include <rendering\quadRenderer2D.h>
#include <rendering\textRenderer2D.h>

#include "control.h"

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
        button(sizef viewportSize);
        ~button();

        std::string getText() { return _text; }
        texture* getTexture() { return _image; }
        color getBackgroundColor() { return _backgroundColor; }
        color getForegroundColor() { return _foregroundColor; }
        font* getFont() { return _font; }
        mouseEventHandler* getClick() { return _click; }

        void setText(std::string value);
        void setImage(texture* value);
        void setBackgroundColor(color value);
        void setForegroundColor(color value) { _foregroundColor = value; }
        void setFont(font* value) { _font = value; }

        void setX(int value) override;
        void setY(int value) override;
        void setZIndex(float value) override;
        void setSize(sizef value) override;
        void setViewportSize(sizef value) override;
        void setIsTopMost(bool value) override;

        void onMouseDown(mouseEventArgs* e) override;
        void onMouseUp(mouseEventArgs* e) override;
        void onMouseEnter(mouseEventArgs* e) override;
        void onMouseLeave(mouseEventArgs* e) override;

        void onRender() override;
    };
}