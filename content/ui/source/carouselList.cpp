#include "carouselList.h"
#include "floatAnimator.h"

namespace phi
{
    carouselList::carouselList(size<GLuint> viewportSize) : control(viewportSize)
    {
        _arrowDownTexture = uiRepository::repository->getResource<texture>("arrow_down.png");
        _arrowUpTexture = uiRepository::repository->getResource<texture>("arrow_up.png");
        _expanded = true;
        _expandButton = new button(viewportSize);
        _expandButton->setToolTipText("Hide");
        _expandButton->setImage(_arrowDownTexture);
        _expandButton->setSize(phi::size<GLuint>(60, 30));
        _expandButton->setForegroundColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
        _expandButton->getClick()->bind<carouselList, &carouselList::expandButtonClick>(this);
        addChild(_expandButton);
        _currentTab = nullptr;
        _selectedItemChanged = new eventHandler<carouselItemEventArgs>();
    }

    carouselList::~carouselList()
    {
    }

    void carouselList::setContentOffset(float value)
    {
        _contentOffset = value;
        updateTabs();
        _expandButton->setY(_y + _contentOffset - _expandButton->getSize().height);
    }

    void carouselList::setX(int value)
    {
        control::setX(value);
        if (_currentTab != nullptr)
            _currentTab->setX(_x);
        _expandButton->setX(_x + _size.width - _expandButton->getSize().width);
    }

    void carouselList::setY(int value)
    {
        control::setY(value);
        if (_currentTab != nullptr)
            _currentTab->setY(_y + _contentOffset);
        _expandButton->setY(_y + _contentOffset - _expandButton->getSize().height);
    }

    void carouselList::setZIndex(float value)
    {
        control::setZIndex(value);
        _expandButton->setZIndex(value + 0.01f);
        if (_currentTab != nullptr)
            _currentTab->setZIndex(_zIndex);
    }

    void carouselList::setSize(size<GLuint> value)
    {
        control::setSize(value);
        setContentOffset(BUTTON_MARGIN);
        _expandButton->setX(_x + _size.width - 100);
    }

    void carouselList::setBackgroundColor(color value)
    {
        _backgroundColor = value;
        if (_currentTab != nullptr)
            _currentTab->setBackgroundColor(value);
        _expandButton->setBackgroundColor(_backgroundColor);
    }

    void carouselList::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        if (_currentTab != nullptr)
            _currentTab->setViewportSize(value);
        _expandButton->setViewportSize(getViewportSize());
    }

    void carouselList::addTab(carouselTab* carouselTab)
    {
        carouselTab->setX(_x);
        carouselTab->setY(_y + _contentOffset);
        carouselTab->setZIndex(_zIndex + 0.01f);
        carouselTab->setSize(size<GLuint>(_size.width, _size.height - _contentOffset, _size.depth));
        carouselTab->setBackgroundColor(_backgroundColor);
        carouselTab->getSelectedItemChanged()->bind<carouselList, &carouselList::carouselTabSelectedItemChanged>(this);

        auto i = _tabs.size();
        _tabs.push_back(carouselTab);

        auto tabButton = new toggleButton(_viewportSize);
        tabButton->setSize(size<GLuint>(120, BUTTON_MARGIN));
        tabButton->setX(_x + i * (120 + 2));
        tabButton->setY(_y);

        auto r = _backgroundColor.r + ((1.0f - _backgroundColor.r) * 0.3f);
        auto g = _backgroundColor.g + ((1.0f - _backgroundColor.g) * 0.3f);
        auto b = _backgroundColor.b + ((1.0f - _backgroundColor.b) * 0.3f);
        auto a = _backgroundColor.a + ((1.0f - _backgroundColor.a) * 0.3f);

        tabButton->setBackgroundColor(color::fromRGBA(r, g, b, _backgroundColor.a));
        tabButton->setForegroundColor(color::white);
        tabButton->setCheckedColor(_backgroundColor);
        tabButton->setText(carouselTab->getName());
        //tabButton->setToolTipText(carouselTab->getName());
        tabButton->setZIndex(_zIndex + 0.01f);
        //tabButton->setImage(carouselTab->getImage());
        _tabsButtons.push_back(tabButton);
        addChild(tabButton);

        if (i == 0)
        {
            _currentTab = carouselTab;
            addChild(_currentTab);
            tabButton->setChecked(true);
        }

        tabButton->getCheckedChanging()->bind<carouselList, &carouselList::tabButtonCheckedChanging>(this);
    }

    void carouselList::setExpanded(bool value)
    {
        if (_expanded == value)
            return;

        _expanded = value;

        if (_expanded)
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, BUTTON_MARGIN, 500, [&](float v) -> void { setContentOffset(v); }));
            //setContentHeight(_size.height - BUTTON_MARGIN);
            _expandButton->setToolTipText("Hide");
            _expandButton->setImage(_arrowDownTexture);
        }
        else
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, _size.height, 500, [&](float v) -> void { setContentOffset(v); }));
            //setContentHeight(0.0f);
            _expandButton->setToolTipText("Show");
            _expandButton->setImage(_arrowUpTexture);
        }
    }

    void carouselList::tabButtonCheckedChanging(controlCancelEventArgs* e)
    {
        auto tabButton = (toggleButton*)e->sender;
        int pos = std::find(_tabsButtons.begin(), _tabsButtons.end(), tabButton) - _tabsButtons.begin();

        if (pos < _tabsButtons.size())
        {
            if (_tabs[pos] == _currentTab && tabButton->getChecked())
                e->cancel = true;
            else
            {
                if (!tabButton->getChecked())
                {
                    if (_currentTab != nullptr)
                        removeChild(_currentTab);

                    _currentTab = _tabs[pos];
                    addChild(_currentTab);
                    setExpanded(true);

                    auto tabsCount = _tabs.size();
                    for (unsigned int i = 0; i < tabsCount; i++)
                    {
                        if (i != pos && _tabsButtons[i]->getChecked())
                            _tabsButtons[i]->setChecked(false);
                    }
                }
            }
        }
    }

    void carouselList::updateTabs()
    {
        auto tabsCount = _tabs.size();
        for (unsigned int i = 0; i < tabsCount; i++)
        {
            carouselTab* tab = _tabs[i];
            tab->setY(_y + _contentOffset);
            auto tabButton = _tabsButtons[i];
            tabButton->setY(_y + _contentOffset - tabButton->getSize().height);
        }
    }

    void carouselList::notifySelectedItemChanged(carouselItemEventArgs e)
    {
        if (_selectedItemChanged->isBound())
            _selectedItemChanged->invoke(e);
    }

    void carouselList::carouselTabSelectedItemChanged(carouselItemEventArgs e)
    {
        if (e.sender->getIsSelected())
            notifySelectedItemChanged(e.sender);
    }

    void carouselList::expandButtonClick(phi::mouseEventArgs* e)
    {
        setExpanded(!_expanded);
    }

    void carouselList::update()
    {
        control::update();
        for (unsigned int i = 0; i < _tabsButtons.size(); i++)
            _tabsButtons[i]->update();

        auto currentItems = _currentTab->getItems();
        auto currentItemsSize = currentItems.size();
        for (unsigned int i = 0; i < currentItemsSize; i++)
            currentItems[i]->update();

        _expandButton->update();
    }

    void carouselList::onRender()
    {
        //_expandButton->render();

        //auto tabsButtonsSize = _tabsButtons.size();
        //for (unsigned int i = 0; i < tabsButtonsSize; i++)
        //    _tabsButtons[i]->render();

        //if (_currentTab == nullptr)
        //    return;

        //glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);

        control::onRender();
        //_currentTab->render();

        glDisable(GL_SCISSOR_TEST);
        //glDisable(GL_BLEND);
    }
}