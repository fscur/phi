#include "button.h"
#include "colorAnimator.h"

namespace phi
{
	button::button(size<GLuint> viewportSize) : control()
    {
        _x = 0;
        _y = 0;
        _size = size<GLuint>();
        _text = "";
        _texture = resourceManager::get()->getTexture("button");
		_textureRenderer = new quadRenderer2D(glm::vec2(0, 0), size<GLuint>(0, 0, 0), viewportSize);
        _textRenderer = new textRenderer2D(viewportSize);
        _font = new font("Consola", 18);
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
        size<int> textSize = _textRenderer->measureSize(_text, _font);
		_textX = (int)(_x + _size.width * 0.5f - textSize.width * 0.5f);
		_textY = (int)(_y + _size.height * 0.5f - textSize.height * 0.5f);
    }

    void button::setX(int value)
    {
        _x = value;
        _textureRenderer->setLocation(glm::vec2(_x, _y));
        _textureRenderer->update();
        updateTextLocation();
    }

    void button::setY(int value)
    {
        _y = value;
        _textureRenderer->setLocation(glm::vec2(_x, _y));
        _textureRenderer->update();
        updateTextLocation();
    }

    void button::setSize(size<GLuint> value)
    {
        _size = value;
        _textureRenderer->setSize(value);
        _textureRenderer->update();
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
        _currentColor = _backgroundColor;
    }

	void button::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _textureRenderer->setViewportSize(getViewportSize());
        _textureRenderer->update();
        _textRenderer->setViewportSize(getViewportSize());
        _textRenderer->update();
    }

    void button::render()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _textureRenderer->render(_texture, _currentColor);
        _textRenderer->render(_text, _font, _foregroundColor, color::transparent, glm::vec2(_textX, _textY));
		glDisable(GL_BLEND);
    }

    void button::onMouseDown(mouseEventArgs e)
    {
        if (getIsMouseOver() && e.leftButtonPressed && !_clickedOver)
        {
            _clickedOver = true;
            _textureRenderer->setLocation(glm::vec2(_x + 1, _y + 1));
			_textureRenderer->setSize(size<GLuint>(_size.width - 2, _size.height - 2));
            _textureRenderer->update();
            colorAnimator::animateColor(&_currentColor, color(glm::min(1.0f, _backgroundColor.r + 0.2f), glm::min(1.0f, _backgroundColor.g + 0.2f), glm::min(1.0f, _backgroundColor.b + 0.2f), 1));
        }
    }

    void button::onMouseUp(mouseEventArgs e)
    {
        if (e.leftButtonPressed && _clickedOver)
        {
            if (getIsMouseOver() && _click->isBound())
                _click->invoke(e);

            _clickedOver = false;
            _textureRenderer->setLocation(glm::vec2(_x, _y));
			_textureRenderer->setSize(_size);
            _textureRenderer->update();

            if (!getIsMouseOver())
                colorAnimator::animateColor(&_currentColor, _backgroundColor);
        }
    }

    void button::onMouseEnter(mouseEventArgs e)
    {
        colorAnimator::animateColor(&_currentColor, color(glm::min(1.0f, _backgroundColor.r + 0.2f), glm::min(1.0f, _backgroundColor.g + 0.2f), glm::min(1.0f, _backgroundColor.b + 0.2f), 1));
    }

    void button::onMouseLeave(mouseEventArgs e)
    {
        colorAnimator::animateColor(&_currentColor, _backgroundColor);
    }
}