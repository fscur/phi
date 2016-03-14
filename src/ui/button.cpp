#include <precompiled.h>
#include "button.h"

#include "colorAnimator.h"

namespace phi
{
    button::button(sizeui viewportSize) : control(viewportSize)
    {
        auto size = sizeui(viewportSize.w, viewportSize.h, viewportSize.d);
        _text = "";
        _image = nullptr;
        _backgroundTexture = uiRepository::textureButton;
        _backgroundRenderer = new quadRenderer2D(vec2(0, 0), _zIndex, sizeui(0, 0, 0), size);
        _overlayRenderer = new quadRenderer2D(vec2(0, 0), _zIndex + 0.02f, sizeui(0, 0, 0), size);
        _overlayColor = color::fromRGBA(1.0f, 1.0f, 1.0f, 0.0f);
        _imageRenderer = new quadRenderer2D(vec2(0, 0), _zIndex + 0.01f, sizeui(0, 0, 0), size);
        _textRenderer = new textRenderer2D(size);
        _font = uiRepository::fontConsolas14;
        _textX = 0;
        _textY = 0;
        _clickedOver = false;
        _click = new mouseEventHandler();
    }

    button::~button()
    {
        safeDelete(_backgroundRenderer);
        safeDelete(_overlayRenderer);
        safeDelete(_textRenderer);
    }

    void button::updateTextLocation()
    {
        sizeui textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.w > _size.w)
            _textX = _x;
        else
            _textX = (int)(_x + _size.w * 0.5f - textSize.w * 0.5f);

        _textY = (int)(_y + _size.h * 0.5f - textSize.h * 0.5f);
    }

    void button::updateImageSize()
    {
        if (_image == nullptr)
            return;

        auto width = _image->w;
        auto height = _image->h;

        if (width < _size.w &&
            height < _size.h)
        {
            _imageRenderer->setSize(sizeui(width, height));
            return;
        }

        auto imageRatio = width / height;

        if (_size.w >= _size.h)
            _imageRenderer->setSize(sizeui(_size.h, _size.h * imageRatio));
        else
            _imageRenderer->setSize(sizeui(_size.w * (GLuint)(1.0f / imageRatio), _size.w));
    }

    void button::updateImageLocation()
    {
        if (_image == nullptr)
            return;

        auto size = _imageRenderer->getSize();
        _imageRenderer->setLocation(vec2(_x + _size.w * 0.5f - size.w * 0.5f, _y + _size.h * 0.5f - size.h * 0.5f));
        _imageRenderer->update();
    }

    void button::animateMouseEnter()
    {
        colorAnimator::animateColor(new colorAnimation(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.3f), 300, easingFunctions::easeOutCubic));
    }

    void button::animateMouseLeave()
    {
        colorAnimator::animateColor(new colorAnimation(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.0f), 500, easingFunctions::easeOutCubic));
    }

    void button::animatePressed()
    {
        colorAnimator::animateColor(new colorAnimation(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.3f), 300, easingFunctions::easeOutCubic));
    }

    void button::animateUnpressed()
    {
        colorAnimator::animateColor(new colorAnimation(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.0f), 500, easingFunctions::easeOutCubic));
    }

    void button::setX(int value)
    {
        control::setX(value);
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        _overlayRenderer->setLocation(vec2(_x, _y));
        _overlayRenderer->update();
        updateImageLocation();
        updateTextLocation();
    }

    void button::setY(int value)
    {
        control::setY(value);
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        _overlayRenderer->setLocation(vec2(_x, _y));
        _overlayRenderer->update();
        updateImageLocation();
        updateTextLocation();
    }

    void button::setZIndex(float value)
    {
        control::setZIndex(value);
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
        _overlayRenderer->setZIndex(_zIndex + 0.02f);
        _overlayRenderer->update();
        _imageRenderer->setZIndex(_zIndex + 0.01f);
        _imageRenderer->update();
    }

    void button::setSize(sizeui value)
    {
        _size = value;
        _backgroundRenderer->setSize(value);
        _backgroundRenderer->update();
        _overlayRenderer->setSize(value);
        _overlayRenderer->update();
        updateImageSize();
        updateImageLocation();
        updateTextLocation();
    }

    void button::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
    }

    void button::setImage(texture* value)
    {
        _image = value;
        updateImageSize();
        updateImageLocation();
    }

    void button::setBackgroundColor(color value)
    {
        _backgroundColor = _currentBackgroundColor = value;
    }

    void button::setViewportSize(sizeui value)
    {
        auto size = getViewportSize();

        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(size);
        _backgroundRenderer->update();
        _overlayRenderer->setViewportSize(size);
        _overlayRenderer->update();
        _textRenderer->setViewportSize(size);
        _textRenderer->update();
    }

    void button::setIsTopMost(bool value)
    {
        control::setIsTopMost(value);
        if (!getIsTopMost())
            animateMouseLeave();
        else
            animateMouseEnter();
    }

    void button::onRender()
    {
        control::onRender();

        control::controlsScissors->pushScissor((float)_x, (float)_y, (float)_size.w, (float)_size.h);
        control::controlsScissors->enable();

        //glPushAttrib(GL_SCISSOR_BIT);
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(_x, (_viewportSize.h - _size.h - _y), _size.w, _size.h);
        renderBackground();
        renderImage();
        renderOverlay();
        renderForeground();
        //glPopAttrib();

        control::controlsScissors->popScissor();
        control::controlsScissors->disable();
    }

    void button::renderBackground()
    {
        _backgroundRenderer->render(_backgroundTexture, _currentBackgroundColor);
    }

    void button::renderOverlay()
    {
        _overlayRenderer->render(_backgroundTexture, _overlayColor);
    }

    void button::renderImage()
    {
        if (_image == nullptr)
            return;

        _imageRenderer->render(_image, color::white);
    }

    void button::renderForeground()
    {
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, vec2(_textX, _textY), _zIndex + 0.03f);
    }

    void button::onMouseDown(mouseEventArgs* e)
    {
        if (getIsMouseOver() && e->leftButtonPressed && !_clickedOver)
        {
            _clickedOver = true;
            //_textureRenderer->setLocation(vec2(_x + 1, _y + 1));
            //_textureRenderer->setSize(sizef(_size.w - 2, _size.h - 2));
            //_textureRenderer->update();
            //_overlayRenderer->setLocation(vec2(_x + 1, _y + 1));
            //_overlayRenderer->setSize(sizef(_size.w - 2, _size.h - 2));
            //_overlayRenderer->update();
            animatePressed();
            e->handled = true;
        }
    }

    void button::onMouseUp(mouseEventArgs* e)
    {
        if (e->leftButtonPressed && _clickedOver)
        {
            if (getIsMouseOver())
            {
                onClick();
                e->sender = this;
                _click->raise(e);
            }

            _clickedOver = false;
            //_textureRenderer->setLocation(vec2(_x, _y));
            //_textureRenderer->setSize(_size);
            //_textureRenderer->update();
            //_overlayRenderer->setLocation(vec2(_x, _y));
            //_overlayRenderer->setSize(_size);
            //_overlayRenderer->update();

            if (!getIsMouseOver())
                animateUnpressed();
        }
    }

    void button::onMouseEnter(mouseEventArgs* e)
    {
        if (getIsTopMost())
            animateMouseEnter();
    }

    void button::onMouseLeave(mouseEventArgs* e)
    {
        animateMouseLeave();
    }
}