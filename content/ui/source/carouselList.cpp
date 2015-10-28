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
        _expandButton->setSize(phi::size<GLuint>(EXPAND_BUTTON_WIDTH, HEADER_HEIGHT));
        _expandButton->setForegroundColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
        _expandButton->getClick()->bind<carouselList, &carouselList::expandButtonClick>(this);
        addChild(_expandButton);
        _currentSubTab = nullptr;
        _currentTab = 0;
        _selectedItemChanged = new eventHandler<carouselItemEventArgs>();
        _tabsScrollOffset = 0.0f;
        _tabsTargetScrollOffset = 0.0f;
        _subTabsScrollOffset = 0.0f;
        _subTabsTargetScrollOffset = 0.0f;
    }

    carouselList::~carouselList()
    {
    }

    void carouselList::setContentOffset(float value)
    {
        _contentOffset = value;
        updateTabs();
        _expandButton->setY(_y + _contentOffset - _expandButton->getSize().height);
        updateScissors();
    }

    void carouselList::setX(int value)
    {
        control::setX(value);
        if (_currentSubTab != nullptr)
            _currentSubTab->setX(_x);
        _expandButton->setX(_x + _size.width - EXPAND_BUTTON_WIDTH);
        updateScissors();
    }

    void carouselList::setY(int value)
    {
        control::setY(value);
        if (_currentSubTab != nullptr)
            _currentSubTab->setY(_y + _contentOffset);
        _expandButton->setY(_y + _contentOffset - HEADER_HEIGHT);
        updateScissors();
    }

    void carouselList::setZIndex(float value)
    {
        control::setZIndex(value);
        _expandButton->setZIndex(value + 0.01f);
        if (_currentSubTab != nullptr)
            _currentSubTab->setZIndex(_zIndex);
    }

    void carouselList::setSize(size<GLuint> value)
    {
        control::setSize(value);
        setContentOffset(HEADER_HEIGHT);
        _expandButton->setX(_x + _size.width - EXPAND_BUTTON_WIDTH);
        updateScissors();
    }

    void carouselList::setBackgroundColor(color value)
    {
        _backgroundColor = value;
        if (_currentSubTab != nullptr)
            _currentSubTab->setBackgroundColor(value);
        _expandButton->setBackgroundColor(_backgroundColor);
    }

    void carouselList::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        if (_currentSubTab != nullptr)
            _currentSubTab->setViewportSize(value);
        _expandButton->setViewportSize(getViewportSize());
    }

    toggleButton* carouselList::createTabButton(std::string text, texture* image, float x, float y, size<GLuint> size)
    {
        auto tabButton = new toggleButton(_viewportSize);
        tabButton->setSize(size);
        tabButton->setX(x);
        tabButton->setY(y);

        auto r = _backgroundColor.r + ((1.0f - _backgroundColor.r) * 0.3f);
        auto g = _backgroundColor.g + ((1.0f - _backgroundColor.g) * 0.3f);
        auto b = _backgroundColor.b + ((1.0f - _backgroundColor.b) * 0.3f);
        auto a = _backgroundColor.a + ((1.0f - _backgroundColor.a) * 0.3f);

        tabButton->setBackgroundColor(color::fromRGBA(r, g, b, _backgroundColor.a));
        tabButton->setForegroundColor(color::white);
        tabButton->setCheckedColor(_backgroundColor);
        tabButton->setText(text);
        tabButton->setImage(image);
        //tabButton->setToolTipText(carouselTab->getName());
        tabButton->setZIndex(_zIndex + 0.01f);

        return tabButton;
    }

    void carouselList::changeCurrentSubTab(carouselTab* subTab)
    {
        auto subTabsList = _tabs[_currentTab];

        if (_currentSubTab != nullptr)
        {
            int lastPos = std::find(subTabsList->begin(), subTabsList->end(), _currentSubTab) - subTabsList->begin();
            auto lastTabButton = (*_subTabsButtons[_currentTab])[lastPos];

            lastTabButton->getCheckedChanged()->bind<carouselList, &carouselList::subTabButtonCheckedChanged>(this);
            lastTabButton->getCheckedChanging()->unbind<carouselList, &carouselList::subTabButtonCheckedChanging>(this);

            removeChild(_currentSubTab);
        }

        _currentSubTab = subTab;

        if (subTab == nullptr)
            return;

        int pos = std::find(subTabsList->begin(), subTabsList->end(), subTab) - subTabsList->begin();
        auto tabButton = (*_subTabsButtons[_currentTab])[pos];

        addChild(subTab);

        tabButton->getCheckedChanged()->unbind<carouselList, &carouselList::subTabButtonCheckedChanged>(this);

        if (!tabButton->getChecked())
            tabButton->setChecked(true);

        tabButton->getCheckedChanging()->bind<carouselList, &carouselList::subTabButtonCheckedChanging>(this);

        auto tabsCount = _tabs[_currentTab]->size();
        for (unsigned int i = 0; i < tabsCount; i++)
        {
            if (i != pos && (*_subTabsButtons[_currentTab])[i]->getChecked())
                (*_subTabsButtons[_currentTab])[i]->setChecked(false);
        }
    }

    int carouselList::addTab(std::string toolTipText, texture* image)
    {
        auto index = _tabs.size();
        _tabs.push_back(new std::vector<carouselTab*>());
        _subTabsButtons.push_back(new std::vector<toggleButton*>());

        auto tabButton = createTabButton(
            "",
            image,
            _tabsScissor.x + _tabsScrollOffset + index * (HEADER_HEIGHT + TAB_MARGIN),
            _tabsScissor.y,
            size<GLuint>(HEADER_HEIGHT, HEADER_HEIGHT));
        tabButton->setZIndex(_zIndex + 0.02f);
        tabButton->setToolTipText(toolTipText);
        _tabsButtons.push_back(tabButton);
        addChild(tabButton);

        if (index == 0)
            tabButton->setChecked(true);

        tabButton->getCheckedChanging()->bind<carouselList, &carouselList::tabButtonCheckedChanging>(this);

        return index;
    }

    void carouselList::addSubTab(int tabIndex, carouselTab* carouselTab)
    {
        carouselTab->setX(_x);
        carouselTab->setY(_y + _contentOffset);
        carouselTab->setZIndex(_zIndex + 0.01f);
        carouselTab->setSize(size<GLuint>(_size.width, _size.height - _contentOffset, _size.depth));
        carouselTab->setBackgroundColor(_backgroundColor);
        carouselTab->getSelectedItemChanged()->bind<carouselList, &carouselList::carouselTabSelectedItemChanged>(this);

        auto i = _tabs[tabIndex]->size();
        _tabs[tabIndex]->push_back(carouselTab);

        auto tabButton = createTabButton(
            carouselTab->getName(),
            nullptr, //carouselTab->getImage(),
            _x + _subTabsScrollOffset + i * (SUB_TAB_WIDTH + TAB_MARGIN),
            _y,
            size<GLuint>(SUB_TAB_WIDTH, HEADER_HEIGHT));
        //tabButton->getCheckedChanging()->bind<carouselList, &carouselList::subTabButtonCheckedChanging>(this);

        _subTabsButtons[tabIndex]->push_back(tabButton);

        if (_currentTab == tabIndex)
        {
            addChild(tabButton);

            if (i == 0)
                changeCurrentSubTab(carouselTab);
        }

        if (i > 0)
            tabButton->getCheckedChanged()->bind<carouselList, &carouselList::subTabButtonCheckedChanged>(this);
    }

    void carouselList::setExpanded(bool value)
    {
        if (_expanded == value)
            return;

        _expanded = value;

        if (_expanded)
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, HEADER_HEIGHT, 500, [&](float v) -> void { setContentOffset(v); }, 0, easingFunctions::easeInOutCubic));
            _expandButton->setToolTipText("Hide");
            _expandButton->setImage(_arrowDownTexture);
        }
        else
        {
            floatAnimator::animateFloat(new floatAnimation(&_contentOffset, _size.height, 1000, [&](float v) -> void { setContentOffset(v); }, 0, easingFunctions::easeOutBounce));
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
            setExpanded(true);
            if (pos == _currentTab && tabButton->getChecked())
                e->cancel = true;
            else
            {
                if (!tabButton->getChecked())
                {
                    for each (auto subTabButton in (*_subTabsButtons[_currentTab]))
                        removeChild(subTabButton);

                    if (_currentSubTab != nullptr)
                        removeChild(_currentSubTab);

                    changeCurrentSubTab(nullptr);

                    _currentTab = pos;

                    for each (auto subTabButton in (*_subTabsButtons[_currentTab]))
                        addChild(subTabButton);

                    if (_tabs[pos]->size() > 0)
                        changeCurrentSubTab((*_tabs[pos])[0]);
                    else
                        _currentSubTab = nullptr;

                    auto tabsCount = _tabs.size();
                    for (unsigned int i = 0; i < tabsCount; i++)
                    {
                        if (i != pos && _tabsButtons[i]->getChecked())
                            _tabsButtons[i]->setChecked(false);
                    }

                    _subTabsScrollOffset = 0.0f;
                    updateTabs();
                }
            }
        }
    }

    void carouselList::subTabButtonCheckedChanging(controlCancelEventArgs* e)
    {
        setExpanded(true);
        e->cancel = true;
    }

    void carouselList::subTabButtonCheckedChanged(controlEventArgs e)
    {
        auto tabButton = (toggleButton*)e.sender;
        if (!tabButton->getChecked())
            return;
        auto subTabsButtonsList = _subTabsButtons[_currentTab];
        auto pos = std::find(subTabsButtonsList->begin(), subTabsButtonsList->end(), tabButton) - subTabsButtonsList->begin();

        changeCurrentSubTab((*_tabs[_currentTab])[pos]);
        setExpanded(true);
    }

    void carouselList::updateTabs()
    {
        if (_tabs.size() == 0)
            return;

        for (unsigned int j = 0; j < _tabs.size(); j++)
        {
            auto subTabsCount = _tabs[j]->size();
            for (unsigned int i = 0; i < subTabsCount; i++)
            {
                carouselTab* tab = (*_tabs[j])[i];
                tab->setY(_y + _contentOffset);
                auto tabButton = (*_subTabsButtons[j])[i];
                tabButton->setX(_x + _subTabsScrollOffset + i * (SUB_TAB_WIDTH + TAB_MARGIN));
                tabButton->setY(_y + _contentOffset - tabButton->getSize().height);
            }
        }

        auto tabsCount = _tabs.size();
        auto totalSize = tabsCount * HEADER_HEIGHT + (tabsCount - 1) * TAB_MARGIN;
        for (unsigned int i = 0; i < tabsCount; i++)
        {
            auto tabButton = _tabsButtons[i];
            float x;
            if (totalSize < TABS_WIDTH)
                x = _tabsScissor.x + TABS_WIDTH * 0.5f - totalSize * 0.5f + i * (HEADER_HEIGHT + TAB_MARGIN);
            else
                x = _tabsScissor.x + _tabsScrollOffset + i * (HEADER_HEIGHT + TAB_MARGIN);
            tabButton->setX(x);
            tabButton->setY(_y + _contentOffset - tabButton->getSize().height);
        }
    }

    void carouselList::updateScissors()
    {
        _subTabsScissor = scissor(_x, _y + _contentOffset - HEADER_HEIGHT, _size.width - EXPAND_BUTTON_WIDTH - REGIONS_MARGIN * 2.0f - TABS_WIDTH, HEADER_HEIGHT);
        _tabsScissor = scissor(_x + _subTabsScissor.x + _subTabsScissor.width + REGIONS_MARGIN, _y + _contentOffset - HEADER_HEIGHT, TABS_WIDTH, HEADER_HEIGHT);
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

    void carouselList::onMouseWheel(mouseEventArgs* e)
    {
        if (!getIsMouseOver())
            return;

        if (_subTabsScissor.intersectsWith(glm::vec2(e->x, e->y)))
        {
            float maxScroll = _size.width - EXPAND_BUTTON_WIDTH - TABS_WIDTH - (float)(_subTabsButtons[_currentTab]->size() * (SUB_TAB_WIDTH + TAB_MARGIN));
            _subTabsTargetScrollOffset = glm::min(0.0f, glm::max(_subTabsTargetScrollOffset - e->wheelDelta * (SUB_TAB_WIDTH + TAB_MARGIN), maxScroll));
            floatAnimator::animateFloat(new floatAnimation(&_subTabsScrollOffset, _subTabsTargetScrollOffset, 250, [&](float v) -> void { updateTabs(); }, 0, easingFunctions::easeOutQuad));
            e->handled = true;
        }
        else if (_tabsScissor.intersectsWith(glm::vec2(e->x, e->y)))
        {
            float maxScroll = _tabsScissor.width - (float)(_tabsButtons.size() * (HEADER_HEIGHT + TAB_MARGIN));
            _tabsTargetScrollOffset = glm::min(0.0f, glm::max(_tabsTargetScrollOffset - e->wheelDelta * (HEADER_HEIGHT + TAB_MARGIN), maxScroll));
            floatAnimator::animateFloat(new floatAnimation(&_tabsScrollOffset, _tabsTargetScrollOffset, 250, [&](float v) -> void { updateTabs(); }, 0, easingFunctions::easeOutQuad));
            e->handled = true;
        }
    }

    void carouselList::onRender()
    {
        control::controlsScissors->pushScissor(_subTabsScissor);
        control::controlsScissors->enable();
        auto subTabsButtons = _subTabsButtons[_currentTab];
        auto size = subTabsButtons->size();
        for (int i = 0; i < size; i++)
            (*subTabsButtons)[i]->render();
        control::controlsScissors->popScissor();
        control::controlsScissors->disable();

        control::controlsScissors->pushScissor(_tabsScissor);
        control::controlsScissors->enable();
        for each (toggleButton* b in _tabsButtons)
            b->render();
        control::controlsScissors->popScissor();
        control::controlsScissors->disable();

        control::controlsScissors->pushScissor(_x, _y + _contentOffset - HEADER_HEIGHT, _size.width, HEADER_HEIGHT);
        control::controlsScissors->enable();
        _expandButton->render();
        control::controlsScissors->popScissor();
        control::controlsScissors->disable();

        if (_currentSubTab != nullptr)
        {
            control::controlsScissors->pushScissor(_x, _y + HEADER_HEIGHT, _size.width, _size.height - HEADER_HEIGHT);
            control::controlsScissors->enable();
            _currentSubTab->render();
            control::controlsScissors->popScissor();
            control::controlsScissors->disable();
        }
    }
}