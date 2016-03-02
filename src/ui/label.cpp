#include "label.h"

namespace phi
{
    label::label(sizef viewportSize) : control(viewportSize)
    {
        _text = "";
        //_texture = uiRepository::texturesRepository->getResource("button.png");
        auto size = sizeui(viewportSize.w, viewportSize.h, viewportSize.d);
        _backgroundRenderer = new quadRenderer2D(vec2(), 0.0f, phi::sizeui(0, 0), size);
        _textRenderer = new textRenderer2D(size);
        //_font = uiRepository::fontsRepository->getResource("Consola_14");
        _textX = 0;
        _textY = 0;
    }

    label::~label()
    {
        safeDelete(_textRenderer);
    }

    void label::updateTextLocation()
    {
        sizef textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.w > _size.w)
            _textX = _x;
        else
            _textX = (int)(_x + _size.w * 0.5f - textSize.w * 0.5f);

        _textY = (int)(_y + _size.h * 0.5f - textSize.h * 0.5f);
    }

    void label::setX(int value)
    {
        _x = value;
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void label::setY(int value)
    {
        _y = value;
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void label::setSize(sizef size)
    {
        _size = size;
        auto sizeu = sizeui(size.w, size.h, size.d);
        _backgroundRenderer->setSize(sizeu);
        _backgroundRenderer->update();
        updateTextLocation();
    }

    void label::setText(std::string value)
    {
        _text = value;
        updateTextLocation();
    }

    void label::setViewportSize(sizef value)
    {
        control::setViewportSize(value);
        auto size = sizeui(value.w, value.h, value.d);
        _textRenderer->setViewportSize(size);
        _textRenderer->update();
    }

    void label::onRender()
    {
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _backgroundRenderer->render(_texture, _backgroundColor);
        _textRenderer->render(_text, _font, _foregroundColor, _backgroundColor, vec2(_textX, _textY), _zIndex + 0.001f);
        //glDisable(GL_BLEND);
    }
}