#include "carouselItem.h"

namespace phi
{
    const float carouselItem::SELECTED_SCALE_PERCENT = 1.05f;

    carouselItem::carouselItem(size<GLuint> viewportSize) : control(viewportSize)
    {
        _texture = uiRepository::repository->getResource<texture>("button.png");
        _selectedTexture = uiRepository::repository->getResource<texture>("button.png");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), 0.0f, size<GLuint>(0, 0, 0), viewportSize);
        _selectedRenderer = new quadRenderer2D(glm::vec2(0, 0), 0.0f, size<GLuint>(0, 0, 0), viewportSize);
        _isSelected = false;
    }

    carouselItem::~carouselItem()
    {
        DELETE(_backgroundRenderer);
    }

    void carouselItem::setX(int value)
    {
        control::setX(value);

        if (_isSelected)
        {
            _backgroundRenderer->setLocation(glm::vec2(_x - _size.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - _size.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
            _selectedRenderer->setLocation(glm::vec2(_x - _size.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - _size.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
        }
        else
        {
            _backgroundRenderer->setLocation(glm::vec2(_x, _y));
            _selectedRenderer->setLocation(glm::vec2(_x, _y));
        }

        _backgroundRenderer->update();
        _selectedRenderer->update();
    }

    void carouselItem::setY(int value)
    {
        control::setY(value);

        if (_isSelected)
        {
            _backgroundRenderer->setLocation(glm::vec2(_x - _size.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - _size.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
            _selectedRenderer->setLocation(glm::vec2(_x - _size.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - _size.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
        }
        else
        {
            _backgroundRenderer->setLocation(glm::vec2(_x, _y));
            _selectedRenderer->setLocation(glm::vec2(_x, _y));
        }

        _backgroundRenderer->update();
        _selectedRenderer->update();
    }

    void carouselItem::setZIndex(float value)
    {
        control::setZIndex(value);
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
        _selectedRenderer->setZIndex(_zIndex + 0.01f);
        _selectedRenderer->update();
    }

    void carouselItem::setSize(size<GLuint> value)
    {
        control::setSize(value);

        if (_isSelected)
        {
            _backgroundRenderer->setSize(size<GLuint>(value.width * SELECTED_SCALE_PERCENT, value.height * SELECTED_SCALE_PERCENT));
            _backgroundRenderer->setLocation(glm::vec2(_x - value.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - value.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
            _selectedRenderer->setSize(size<GLuint>(value.width * SELECTED_SCALE_PERCENT, value.height * SELECTED_SCALE_PERCENT));
            _selectedRenderer->setLocation(glm::vec2(_x - value.width * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f, _y - value.height * (SELECTED_SCALE_PERCENT - 1.0f) * 0.5f));
        }
        else
        {
            _backgroundRenderer->setSize(value);
            _selectedRenderer->setSize(value);
            _backgroundRenderer->setLocation(glm::vec2(_x, _y));
            _selectedRenderer->setLocation(glm::vec2(_x, _y));
        }

        _backgroundRenderer->update();
        _selectedRenderer->update();
    }

    void carouselItem::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
        _selectedRenderer->setViewportSize(getViewportSize());
        _selectedRenderer->update();
    }

    void carouselItem::setIsSelected(bool value)
    {
        _isSelected = value;

        if (_isSelected)
        {
            _backgroundRenderer->setSize(size<GLuint>(_size.width * 1.1f, _size.height * 1.1f));
            _backgroundRenderer->setLocation(glm::vec2(_x - _size.width * 0.1f * 0.5f, _y - _size.height * 0.1f * 0.5f));
            _selectedRenderer->setSize(size<GLuint>(_size.width * 1.1f, _size.height * 1.1f));
            _selectedRenderer->setLocation(glm::vec2(_x - _size.width * 0.1f * 0.5f, _y - _size.height * 0.1f * 0.5f));
        }
        else
        {
            _backgroundRenderer->setSize(_size);
            _selectedRenderer->setSize(_size);
            _backgroundRenderer->setLocation(glm::vec2(_x, _y));
            _selectedRenderer->setLocation(glm::vec2(_x, _y));
        }

        _backgroundRenderer->update();
        _selectedRenderer->update();
    }

    void carouselItem::render()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _backgroundRenderer->render(_texture, color::white);

        if (_isSelected)
            _selectedRenderer->render(_selectedTexture, color::fromRGBA(0.0f, 0.0f, 1.0f, 0.15f));

        glDisable(GL_BLEND);
    }
}