#include "button.h"
#include "colorAnimator.h"

namespace phi
{
    button::button(size<GLuint> viewportSize) : control(viewportSize)
    {
        _text = "";
        _texture = uiRepository::repository->getResource<texture>("button.png");
        _textureRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex, size<GLuint>(0, 0, 0), viewportSize);
        _overlayRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.01f, size<GLuint>(0, 0, 0), viewportSize);
        _overlayColor = color::fromRGBA(1.0f, 1.0f, 1.0f, 0.0f);
        _textRenderer = new textRenderer2D(viewportSize);
        _font = uiRepository::repository->getResource<font>("Consola_18");
        _textX = 0;
        _textY = 0;
        _clickedOver = false;
        _click = new mouseEventHandler();
    }

    button::~button()
    {
        DELETE(_textureRenderer);
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
        _textureRenderer->setLocation(glm::vec2(_x, _y));
        _textureRenderer->update();
        _overlayRenderer->setLocation(glm::vec2(_x, _y));
        _overlayRenderer->update();
        updateTextLocation();
    }

    void button::setY(int value)
    {
        _y = value;
        _textureRenderer->setLocation(glm::vec2(_x, _y));
        _textureRenderer->update();
        _overlayRenderer->setLocation(glm::vec2(_x, _y));
        _overlayRenderer->update();
        updateTextLocation();
    }

    void button::setZIndex(float value)
    {
        control::setZIndex(value);
        _textureRenderer->setZIndex(_zIndex);
        _textureRenderer->update();
        _overlayRenderer->setZIndex(_zIndex + 0.01f);
        _overlayRenderer->update();
    }

    void button::setSize(size<GLuint> value)
    {
        _size = value;
        _textureRenderer->setSize(value);
        _textureRenderer->update();
        _overlayRenderer->setSize(value);
        _overlayRenderer->update();
        updateTextLocation();
    }

    void button::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
    }

    void button::setBackgroundColor(color value)
    {
        _backgroundColor = value;
    }

    void button::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _textureRenderer->setViewportSize(getViewportSize());
        _textureRenderer->update();
        _overlayRenderer->setViewportSize(getViewportSize());
        _overlayRenderer->update();
        _textRenderer->setViewportSize(getViewportSize());
        _textRenderer->update();
    }

    void button::render()
    {
        glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);
        _textureRenderer->render(_texture, _backgroundColor);
        _overlayRenderer->render(_texture, _overlayColor);
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, glm::vec2(_textX, _textY), _zIndex + 0.02f);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);
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
            if (getIsMouseOver() && _click->isBound())
                _click->invoke(e);

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
        animateMouseEnter();
    }

    void button::onMouseLeave(mouseEventArgs* e)
    {
        animateMouseLeave();
    }
}