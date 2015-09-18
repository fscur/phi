#include "carouselList.h"
#include "floatAnimator.h"

namespace phi
{
    carouselList::carouselList(size<GLuint> viewportSize) : control(viewportSize)
    {
        _texture = uiRepository::repository->getResource<texture>("button.png");
        _backgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), 0.0f, size<GLuint>(0, 0, 0), viewportSize);
        _scrollOffset = 0.0f;
        _targetScrollOffset = 0.0f;
        _selectedItemChanged = new eventHandler<carouselItemEventArgs>();
        _expanded = true;
        _expandButton = new button(viewportSize);
        _expandButton->setText("Hide");
        _expandButton->setSize(phi::size<GLuint>(100, BUTTON_MARGIN));
        _expandButton->setForegroundColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
        _expandButton->getClick()->bind<carouselList, &carouselList::expandButtonClick>(this);
    }

    carouselList::~carouselList()
    {
        DELETE(_backgroundRenderer);
    }

    void carouselList::setContentHeight(float value)
    {
        _contentOffset = value;
        _backgroundRenderer->setSize(size<GLuint>(_size.width, _size.height - _contentOffset, _size.depth));
        _backgroundRenderer->setLocation(glm::vec2(_x, _y + _contentOffset));
        _backgroundRenderer->update();
        _expandButton->setY(_y + _contentOffset - _expandButton->getSize().height + 1);
        updateItems();
    }

    void carouselList::setX(int value)
    {
        control::setX(value);
        _backgroundRenderer->setLocation(glm::vec2(_x, _y + _contentOffset));
        _backgroundRenderer->update();
        _expandButton->setX(_x + _size.width - 100);
    }

    void carouselList::setY(int value)
    {
        control::setY(value);
        _backgroundRenderer->setLocation(glm::vec2(_x, _y + _contentOffset));
        _backgroundRenderer->update();
        _expandButton->setY(_y + _contentOffset - _expandButton->getSize().height + 1);
    }

    void carouselList::setZIndex(float value)
    {
        control::setZIndex(value);
        _backgroundRenderer->setZIndex(_zIndex);
        _backgroundRenderer->update();
    }

    void carouselList::setSize(size<GLuint> value)
    {
        control::setSize(value);
        setContentHeight(BUTTON_MARGIN);
        _expandButton->setX(_x + _size.width - 100);
    }

    void carouselList::setBackgroundColor(color value)
    {
        _backgroundColor = value;
        _expandButton->setBackgroundColor(_backgroundColor);
    }

    void carouselList::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _backgroundRenderer->setViewportSize(getViewportSize());
        _backgroundRenderer->update();
        _expandButton->setViewportSize(getViewportSize());
    }

    void carouselList::setSelectedItem(carouselItem* item)
    {
        for (carouselItem* item2 : _items)
            item2->setIsSelected(false);

        item->setIsSelected(true);

        float itemsHeight = _size.height - 2.0f * ITEM_MARGIN;
        float maxScroll = _size.width - (_items.size() * (itemsHeight + ITEM_MARGIN) + ITEM_MARGIN);
        _targetScrollOffset = glm::min(0.0f, glm::max(_targetScrollOffset - (float)item->getX() + _size.width * 0.5f - itemsHeight * 0.5f, maxScroll));
        floatAnimator::animateFloat(&_scrollOffset, _targetScrollOffset, 250);
        updateItems();
    }

    void carouselList::addCarouselItem(carouselItem* carouselItem)
    {
        _items.push_back(carouselItem);
        GLuint height = _size.height - ITEM_MARGIN * 2.0f;
        carouselItem->setSize(size<GLuint>(height, height));
        carouselItem->setX(_x + _scrollOffset + (_items.size() - 1) * (height + ITEM_MARGIN) + ITEM_MARGIN);
        carouselItem->setY(_y + ITEM_MARGIN);
        carouselItem->setZIndex(_zIndex + 0.01f);

        carouselItem->getIsSelectedChanged()->bind<carouselList, &carouselList::carouselItemIsSelectedChanged>(this);
    }

    void carouselList::updateItems()
    {
        GLuint height = _size.height - BUTTON_MARGIN - ITEM_MARGIN * 2.0f;
        auto itemsCount = _items.size();

        for (unsigned int i = 0; i < itemsCount; i++)
        {
            carouselItem* item = _items[i];
            item->setSize(size<GLuint>(height, height));
            item->setX(_x + _scrollOffset + i * (height + ITEM_MARGIN) + ITEM_MARGIN);
            item->setY(_y + _contentOffset + ITEM_MARGIN);
            item->setZIndex(_zIndex + 0.01f);
        }
    }

    void carouselList::expandButtonClick(phi::mouseEventArgs* e)
    {
        if (_expanded)
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, _size.height, 500, [&](float v) -> void { setContentHeight(v); }));
            //setContentHeight(0.0f);
            _expandButton->setText("Show");
            _expanded = false;
        }
        else
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, BUTTON_MARGIN, 500, [&](float v) -> void { setContentHeight(v); }));
            //setContentHeight(_size.height - BUTTON_MARGIN);
            _expandButton->setText("Hide");
            _expanded = true;
        }
    }

    void carouselList::notifySelectedItemChanged(carouselItemEventArgs e)
    {
        if (_selectedItemChanged->isBound())
            _selectedItemChanged->invoke(e);
    }

    void carouselList::carouselItemIsSelectedChanged(carouselItemEventArgs e)
    {
        if (e.sender->getIsSelected())
            notifySelectedItemChanged(e.sender);
    }

    void carouselList::onMouseMove(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        if (!_expandButton->isPointInside(e->x, e->y))
        {
            if (_expandButton->getIsMouseOver())
                _expandButton->notifyMouseLeave(e);

            return;
        }

        if (!_expandButton->getIsMouseOver())
            _expandButton->notifyMouseEnter(e);

        _expandButton->notifyMouseMove(e);
    }

    void carouselList::onMouseDown(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        if (_expandButton->getIsMouseOver())
        {
            _expandButton->notifyMouseDown(e);
            return;
        }

        if (!_expanded)
            return;

        auto itemsCount = _items.size();
        for (carouselItem* item : _items)
        {
            auto x = item->getX();
            auto y = item->getY();
            auto size = item->getSize();
            if (e->x >= x && e->x <= x + (int)size.width && e->y >= y && e->y <= y + (int)size.height)
            {
                for (carouselItem* item2 : _items)
                    item2->setIsSelected(false);

                item->setIsSelected(true);
                break;
            }
        }

        e->handled = true;
    }

    void carouselList::onMouseUp(mouseEventArgs* e)
    {
        _expandButton->notifyMouseUp(e);
    }

    void carouselList::onMouseWheel(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        if (!_expanded)
            return;

        float itemsHeight = _size.height - 2.0f * ITEM_MARGIN;
        float maxScroll = _size.width - (_items.size() * (itemsHeight + ITEM_MARGIN) + ITEM_MARGIN);
        _targetScrollOffset = glm::min(0.0f, glm::max(_targetScrollOffset - e->wheelDelta * (itemsHeight + ITEM_MARGIN), maxScroll));
        //_scrollOffset = glm::min(0.0f, glm::max(_scrollOffset - e->wheelDelta * 50.0f, maxScroll));
        floatAnimator::animateFloat(&_scrollOffset, _targetScrollOffset, 250);
        updateItems();
        e->handled = true;
    }

    void carouselList::render()
    {
        updateItems();

        _expandButton->render();

        glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);

        _backgroundRenderer->render(_texture, _backgroundColor);
        for (carouselItem* item : _items)
            item->render();

        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);
    }
}