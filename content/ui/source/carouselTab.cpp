#include "carouselTab.h"
#include "floatAnimator.h"

namespace phi
{
    carouselTab::carouselTab(size<GLuint> viewportSize) : control(viewportSize)
    {
        _backgroundTexture = uiRepository::repository->getResource<texture>("button.png");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), 0.0f, size<GLuint>(0, 0, 0), viewportSize);
        _scrollOffset = 0.0f;
        _targetScrollOffset = 0.0f;
        _selectedItemChanged = new eventHandler<carouselItemEventArgs>();
    }

    carouselTab::~carouselTab()
    {
        DELETE(_backgroundRenderer);
    }

    void carouselTab::setX(int value)
    {
        control::setX(value);
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        updateItems();
    }

    void carouselTab::setY(int value)
    {
        control::setY(value);
        _backgroundRenderer->setLocation(glm::vec2(_x, _y));
        _backgroundRenderer->update();
        updateItems();
    }

    void carouselTab::setZIndex(float value)
    {
        control::setZIndex(value);
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
    }

    void carouselTab::setSize(size<GLuint> value)
    {
        control::setSize(value);
        _backgroundRenderer->setSize(value);
        _backgroundRenderer->update();
    }

    void carouselTab::setBackgroundColor(color value)
    {
        _backgroundColor = value;
    }

    void carouselTab::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
    }

    void carouselTab::notifySelectedItemChanged(carouselItemEventArgs e)
    {
        if (_selectedItemChanged->isBound())
            _selectedItemChanged->invoke(e);
    }

    void carouselTab::carouselItemIsSelectedChanged(carouselItemEventArgs e)
    {
        if (e.sender->getIsSelected())
        {
            for (carouselItem* item : _items)
                if (item != e.sender)
                    item->setIsSelected(false);

            notifySelectedItemChanged(e.sender);
        }
    }

    void carouselTab::addCarouselItem(carouselItem* carouselItem)
    {
        _items.push_back(carouselItem);
        GLuint height = _size.height - ITEM_MARGIN * 2.0f;
        carouselItem->setSize(size<GLuint>(height, height));
        carouselItem->setX(_x + _scrollOffset + (_items.size() - 1) * (height + ITEM_MARGIN) + ITEM_MARGIN);
        carouselItem->setY(_y + ITEM_MARGIN);
        carouselItem->setZIndex(_zIndex + 0.01f);
        carouselItem->getIsSelectedChanged()->bind<carouselTab, &carouselTab::carouselItemIsSelectedChanged>(this);
        addChild(carouselItem);
    }

    void carouselTab::setSelectedItem(carouselItem* item)
    {
        for (carouselItem* item2 : _items)
            item2->setIsSelected(false);

        item->setIsSelected(true);

        float itemsHeight = _size.height - 2.0f * ITEM_MARGIN;
        float maxScroll = _size.width - (_items.size() * (itemsHeight + ITEM_MARGIN) + ITEM_MARGIN);
        _targetScrollOffset = glm::min(0.0f, glm::max(_targetScrollOffset - (float)item->getX() + _size.width * 0.5f - itemsHeight * 0.5f, maxScroll));
        floatAnimator::animateFloat(new floatAnimation(&_scrollOffset, _targetScrollOffset, 250, nullptr, 0, easingFunctions::easeOutQuad));
        updateItems();
    }

    void carouselTab::updateItems()
    {
        GLuint height = getItemHeight();
        auto itemsCount = _items.size();

        for (unsigned int i = 0; i < itemsCount; i++)
        {
            carouselItem* item = _items[i];
            item->setSize(size<GLuint>(height, height));
            item->setX(_x + _scrollOffset + i * (height + ITEM_MARGIN) + ITEM_MARGIN);
            item->setY(_y + ITEM_MARGIN);
            item->setZIndex(_zIndex + 0.01f);
        }
    }

    float carouselTab::getItemHeight()
    {
        return _size.height - ITEM_MARGIN * 2.0f;
    }

    void carouselTab::onMouseWheel(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        float itemsHeight = getItemHeight();
        float maxScroll = _size.width - (_items.size() * (itemsHeight + ITEM_MARGIN) + ITEM_MARGIN);
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