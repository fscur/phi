#include "button.h"
#include "colorAnimator.h"

namespace phi
{
    button::button(size<GLuint> viewportSize) : control(viewportSize)
    {
        _text = "";
        _image = nullptr;
        _backgroundTexture = uiRepository::repository->getResource<texture>("button.png");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex, size<GLuint>(0, 0, 0), viewportSize);
        _overlayRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.02f, size<GLuint>(0, 0, 0), viewportSize);
        _overlayColor = color::fromRGBA(1.0f, 1.0f, 1.0f, 0.0f);
        _imageRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.01f, size<GLuint>(0, 0, 0), viewportSize);
        _textRenderer = new textRenderer2D(viewportSize);
        _font = uiRepository::repository->getResource<font>("Consola_14");
        _textX = 0;
        _textY = 0;
        _clickedOver = false;
        _click = new mouseEventHandler();
    }

    button::~button()
    {
        DELETE(_backgroundRenderer);
        DELETE(_overlayRenderer);
        DELETE(_textRenderer);
    }

    void button::updateTextLocation()
    {
        size<unsigned int> textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.width > _size.width)
            _textX = _x;
        else
            _textX = (int)(_x + _size.width * 0.5f - textSize.width * 0.5f);

        _textY = (int)(_y + _size.height * 0.5f - textSize.height * 0.5f);
    }

    void button::updateImageLocation()
    {
        if (_image == nullptr)
            return;

        auto size = _image->getSize();
        _imageRenderer->setLocation(glm::vec2(_x + _size.width * 0.5f - size.width * 0.5f, _y + _size.height * 0.5f - size.height * 0.5f));
        _imageRenderer->update();
    }

    void button::animateMouseEnter()
    {
        colorAnimator::animateColor(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.3f), 300);
    }

    void button::animateMouseLeave()
    {
        colorAnimator::animateColor(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.0f), 500);
    }

    void button::animatePressed()
    {
        colorAnimator::animateColor(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.3f), 300);
    }

    void button::animateUnpressed()
    {
        colorAnimator::animateColor(&_overlayColor, color(1.0f, 1.0f, 1.0f, 0.0f), 500);
    }

    void button::setX(int value)
    {
        _x = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        _overlayRenderer->setLocation(glm::vec2(_x, _y));
        _overlayRenderer->update();
        updateImageLocation();
        updateTextLocation();
    }

    void button::setY(int value)
    {
        _y = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        _overlayRenderer->setLocation(glm::vec2(_x, _y));
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

    void button::setSize(size<GLuint> value)
    {
        _size = value;
        _backgroundRenderer->setSize(value);
        _backgroundRenderer->update();
        _overlayRenderer->setSize(value);
        _overlayRenderer->update();
        updateImageLocation();
        updateTextLocation();

        if (_image != nullptr)
            _imageRenderer->setSize(_image->getSize());
    }

    void button::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
    }

    void button::setImage(texture* value)
    {
        _image = value;
        if (_image != nullptr)
        {
            _imageRenderer->setSize(_image->getSize());
            updateImageLocation();
        }
    }

    void button::setBackgroundColor(color value)
    {
        _backgroundColor = _currentBackgroundColor = value;
    }

    void button::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
        _overlayRenderer->setViewportSize(getViewportSize());
        _overlayRenderer->update();
        _textRenderer->setViewportSize(getViewportSize());
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
        
        control::controlsScissors->pushScissor(_x, _y, _size.width, _size.height);
        control::controlsScissors->enable();

        //glPushAttrib(GL_SCISSOR_BIT);
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);
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
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, glm::vec2(_textX, _textY), _zIndex + 0.03f);
    }

    void button::onMouseDown(mouseEventArgs* e)
    {
        if (getIsMouseOver() && e->leftButtonPressed && !_clickedOver)
        {
            _clickedOver = true;
            //_textureRenderer->setLocation(glm::vec2(_x + 1, _y + 1));
            //_textureRenderer->setSize(size<GLuint>(_size.width - 2, _size.height - 2));
            //_textureRenderer->update();
            //_overlayRenderer->setLocation(glm::vec2(_x + 1, _y + 1));
            //_overlayRenderer->setSize(size<GLuint>(_size.width - 2, _size.height - 2));
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
                if (_click->isBound())
                    _click->invoke(e);
            }

            _clickedOver = false;
            //_textureRenderer->setLocation(glm::vec2(_x, _y));
            //_textureRenderer->setSize(_size);
            //_textureRenderer->update();
            //_overlayRenderer->setLocation(glm::vec2(_x, _y));
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