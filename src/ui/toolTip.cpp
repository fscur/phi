#include <precompiled.h>
#include "toolTip.h"

#include "uiRepository.h"
#include "floatAnimator.h"
#include "colorAnimator.h"

namespace phi
{
    toolTip::toolTip(sizeui viewportSize)
    {
        _viewportSize = viewportSize;

        auto size = sizeui(viewportSize.w, viewportSize.h, viewportSize.d);
        _backgroundRenderer = new quadRenderer2D(vec2(), 30.0f, sizeui(), size);
        _backgroundTexture = uiRepository::textureButton;

        _foregroundRenderer = new textRenderer2D(size);
        _font = uiRepository::fontConsolas14;
        _textColor = color::white;
    }

    toolTip::~toolTip()
    {
    }

    void toolTip::show(const std::string text, const vec2 location, const sizeui rect)
    {
        _text = text;

        auto textSize = _foregroundRenderer->measureSize(_text, _font);
        _size = sizeui((GLuint)(textSize.w + 2.0f * TEXT_MARGIN), (GLuint)(textSize.h + 2.0f * TEXT_MARGIN));

        float textY;
        float targetHeight;
        if (location.y + _size.h > _viewportSize.h)
        {
            _location = vec2(location.x, location.y - rect.h);
            textY = _location.y - _size.h + TEXT_MARGIN;
            targetHeight = -(float)_size.h;
        }
        else
        {
            _location = location;
            textY = _location.y + TEXT_MARGIN;
            targetHeight = (float)_size.h;
        }

        auto textX = glm::max(0.0f, _location.x - (_size.w - glm::min(_size.w * 0.5f, _viewportSize.w - _location.x))) + TEXT_MARGIN;
        _textLocation = vec2(textX, textY);

        _backgroundRenderer->setSize(sizeui((GLuint)3.0f, (GLuint)0.0f));
        _backgroundRenderer->setLocation(_location);
        _backgroundRenderer->update();

        float* w = new float(3.0f);
        floatAnimator::animateFloat(new phi::floatAnimation(w, (float)_size.w, 150, [&](float w) -> void
        {
            auto s = _backgroundRenderer->getSize();
            _backgroundRenderer->setSize(sizeui((GLuint)w, s.h));
            auto l = _backgroundRenderer->getLocation();
            auto x = glm::max(0.0f, _location.x - (w - glm::min(w * 0.5f, _viewportSize.w - _location.x)));
            _backgroundRenderer->setLocation(vec2(x, l.y));
            _backgroundRenderer->update();
        }, 150, easingFunctions::easeOutQuad));

        float* h = new float(0.0f);
        floatAnimator::animateFloat(new phi::floatAnimation(h, targetHeight, 100, [&](float h) -> void
        {
            auto s = _backgroundRenderer->getSize();
            _backgroundRenderer->setSize(sizeui(s.w, (GLuint)abs(h)));

            if (h < 0.0f)
            {
                auto l = _backgroundRenderer->getLocation();
                _backgroundRenderer->setLocation(vec2(l.x, _location.y + h));
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
        glScissor((GLuint)l.x, (GLuint)(_viewportSize.h - s.h - l.y), (GLuint)s.w, (GLuint)s.h);

        _backgroundRenderer->render(_backgroundTexture, color::black);
        _foregroundRenderer->render(_text, _font, _textColor, color::transparent, _textLocation, 30.1f);

        glPopAttrib();
    }
}