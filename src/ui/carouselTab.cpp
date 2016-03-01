#include "carouselTab.h"

#include "floatAnimator.h"

namespace phi
{
    carouselTab::carouselTab(sizef viewportSize) : control(viewportSize)
    {
        //_backgroundTexture = uiRepository::texturesRepository->getResource("button.png");
        _backgroundRenderer = new quadRenderer2D(vec2(0, 0), 0.0f, sizef(0, 0, 0), viewportSize);
        _scrollOffset = 0.0f;
        _targetScrollOffset = 0.0f;
    }

    carouselTab::~carouselTab()
    {
        safeDelete(_backgroundRenderer);
    }

    void carouselTab::setX(int value)
    {
        control::setX(value);
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        updateItems();
    }

    void carouselTab::setY(int value)
    {
        control::setY(value);
        _backgroundRenderer->setLocation(vec2(_x, _y));
        _backgroundRenderer->update();
        updateItems();
    }

    void carouselTab::setZIndex(float value)
    {
        control::setZIndex(value);
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
    }

    void carouselTab::setSize(sizef value)
    {
        control::setSize(value);
        _backgroundRenderer->setSize(value);
        _backgroundRenderer->update();
    }

    void carouselTab::setBackgroundColor(color value)
    {
        _backgroundColor = value;
    }

    void carouselTab::setViewportSize(sizef value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
    }

    void carouselTab::addCarouselItem(carouselItem* item)
    {
        auto height = _size.h - ITEM_MARGIN * 2.0f;
        item->setSize(sizef((GLuint)height, (GLuint)height));
        item->setX((int)(_x + _scrollOffset + (_items.size() - 1) * (height + ITEM_MARGIN) + ITEM_MARGIN));
        item->setY((int)(_y + ITEM_MARGIN));
        item->setZIndex(_zIndex + 0.01f);
        item->setBackgroundColor(color::fromRGBA(0.0f, 0.0f, 0.0f, 0.0f));
        //b->getClick()->bind([](mouseEventArgs* e) -> void {});

        _items.push_back(item);
        addChild(item);
    }

    void carouselTab::updateItems()
    {
        auto height = getItemHeight();
        auto itemsCount = _items.size();

        for (unsigned int i = 0; i < itemsCount; i++)
        {
            button* item = _items[i];
            item->setSize(sizef((GLuint)height, (GLuint)height));
            item->setX((int)(_x + _scrollOffset + i * (height + ITEM_MARGIN) + ITEM_MARGIN));
            item->setY(_y + ITEM_MARGIN);
            item->setZIndex(_zIndex + 0.01f);
        }
    }

    float carouselTab::getItemHeight()
    {
        return _size.h - ITEM_MARGIN * 2.0f;
    }

    void carouselTab::onMouseWheel(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        float itemsHeight = getItemHeight();
        float maxScroll = _size.w - (_items.size() * (itemsHeight + ITEM_MARGIN) + ITEM_MARGIN);
        _targetScrollOffset = glm::min(0.0f, glm::max(_targetScrollOffset - e->wheelDelta * (itemsHeight + ITEM_MARGIN), maxScroll));
        floatAnimator::animateFloat(new floatAnimation(&_scrollOffset, _targetScrollOffset, 250, [&](float v) -> void { updateItems(); }, 0, easingFunctions::easeOutQuad));
        e->handled = true;
    }

    void carouselTab::onRender()
    {
        _backgroundRenderer->render(_backgroundTexture, _backgroundColor);
        control::onRender();
    }
}