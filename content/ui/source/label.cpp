#include "label.h"

namespace phi
{
    label::label(size<GLuint> viewportSize) : control()
    {
        _x = 0;
        _y = 0;
        _size = 0;
        _text = "";
        _texture = resourceManager::get()->getTexture("button");
        _textRenderer = new textRenderer2D(viewportSize);
        _font = new font("Consola", 14);
        _textX = 0;
        _textY = 0;
    }

    label::~label()
    {
        DELETE(_textRenderer);
    }

    void label::updateTextLocation()
    {
        size<int> textSize = _textRenderer->measureSize(_text, _font);
        _textX = _x;
		_textY = _y;
    }

    void label::setX(int value)
    {
        _x = value;
        updateTextLocation();
    }

    void label::setY(int value)
    {
        _y = value;
        updateTextLocation();
    }

	void label::setSize(size<GLuint> size)
    {
        _size = size;
        updateTextLocation();
    }

    void label::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
    }

	void label::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _textRenderer->setViewportSize(getViewportSize());
        _textRenderer->update();
    }

    void label::render()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_textRenderer->render(_text, _font, _foregroundColor, _backgroundColor, glm::vec2(_textX, _textY));
        glDisable(GL_BLEND);
    }
}