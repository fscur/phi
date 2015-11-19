#include "toolTip.h"
#include "uiRepository.h"
#include "floatAnimator.h"
#include "colorAnimator.h"

namespace phi
{
    toolTip::toolTip(size<GLuint> viewportSize)
    {
        _viewportSize = viewportSize;

        _backgroundRenderer = new quadRenderer2D(glm::vec2(), 30.0f, size<GLuint>(), viewportSize);
        _backgroundTexture = uiRepository::repository->getResource<texture>("button.png");

        _foregroundRenderer = new textRenderer2D(viewportSize);
        _font = uiRepository::repository->getResource<font>("Consola_14");
        _textColor = color::white;
    }

    void toolTip::show(const std::string text, const glm::vec2 location, const size<GLuint> rect)
    {
        _text = text;

        auto textSize = _foregroundRenderer->measureSize(_text, _font);
        _size = size<GLuint>(textSize.width + 2.0f * TEXT_MARGIN, textSize.height + 2.0f * TEXT_MARGIN);

        float textY;
        float targetHeight;
        if (location.y + _size.height > _viewportSize.height)
        {
            _location = glm::vec2(location.x, location.y - rect.height);
            textY = _location.y - _size.height + TEXT_MARGIN;
            targetHeight = -(float)_size.height;
        }
        else
        {
            _location = location;
            textY = _location.y + TEXT_MARGIN;
            targetHeight = _size.height;
        }

        auto textX = glm::max(0.0f, _location.x - (_size.width - glm::min(_size.width * 0.5f, _viewportSize.width - _location.x))) + TEXT_MARGIN;
        _textLocation = glm::vec2(textX, textY);

        _backgroundRenderer->setSize(size<GLuint>(3.0f, 0.0f));
        _backgroundRenderer->setLocation(_location);
        _backgroundRenderer->update();

        float* w = new float(3.0f);
        floatAnimator::animateFloat(new phi::floatAnimation(w, _size.width, 150, [&](float w) -> void
        {
            auto s = _backgroundRenderer->getSize();
            _backgroundRenderer->setSize(size<GLuint>(w, s.height));
            auto l = _backgroundRenderer->getLocation();
            auto x = glm::max(0.0f, _location.x - (w - glm::min(w * 0.5f, _viewportSize.width - _location.x)));
            _backgroundRenderer->setLocation(glm::vec2(x, l.y));
            _backgroundRenderer->update();
        }, 150, easingFunctions::easeOutQuad));

        float* h = new float(0.0f);
        floatAnimator::animateFloat(new phi::floatAnimation(h, targetHeight, 100, [&](float h) -> void
        {
            auto s = _backgroundRenderer->getSize();
            _backgroundRenderer->setSize(size<GLuint>(s.width, glm::abs(h)));

            if (h < 0.0f)
            {
                auto l = _backgroundRenderer->getLocation();
                _backgroundRenderer->setLocation(glm::vec2(l.x, _location.y + h));
            }

            _backgroundRenderer->update();
        }, 0, easingFunctions::easeOutQuad));

        _textColor = color::fromRGBA(1.0f, 1.0f, 1.0f, 0.0f);
        colorAnimator::animateColor(&_textColor, color::white, 350);
    }

    void toolTip::render()
    {
        glPushAttrib(GL_SCISSOR_BIT);
        glEnable(GL_SCISSOR_TEST);
        auto l = _backgroundRenderer->getLocation();
        auto s = _backgroundRenderer->getSize();
        glScissor(l.x, _viewportSize.height - s.height - l.y, s.width, s.height);

        _backgroundRenderer->render(_backgroundTexture, color::black);
        _foregroundRenderer->render(_text, _font, _textColor, color::transparent, _textLocation, 30.1f);

        glPopAttrib();
    }
}