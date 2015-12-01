#include "phi/ui/label.h"

namespace phi
{
    label::label(size<GLuint> viewportSize) : control(viewportSize)
    {
        _text = "";
        _texture = uiRepository::repository->getResource<texture>("button.png");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(), 0.0f, phi::size<GLuint>(0, 0), viewportSize);
        _textRenderer = new textRenderer2D(viewportSize);
        _font = uiRepository::repository->getResource<font>("Consola_14");
        _textX = 0;
        _textY = 0;
    }

    label::~label()
    {
        safeDelete(_textRenderer);
    }

    void label::updateTextLocation()
    {
        size<unsigned int> textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.width > _size.width)
            _textX = _x;
        else
            _textX = (int)(_x + _size.width * 0.5f - textSize.width * 0.5f);

        _textY = (int)(_y + _size.height * 0.5f - textSize.height * 0.5f);
    }

    void label::setX(int value)
    {
        _x = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void label::setY(int value)
    {
        _y = value;
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        updateTextLocation();
    }

	void label::setSize(size<GLuint> size)
    {
        _size = size;
        _backgroundRenderer->setSize(size);
        _backgroundRenderer->update();
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

    void label::onRender()
    {
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _backgroundRenderer->render(_texture, _backgroundColor);
        _textRenderer->render(_text, _font, _foregroundColor, _backgroundColor, glm::vec2(_textX, _textY), _zIndex + 0.001f);
        //glDisable(GL_BLEND);
    }
}