#ifndef _PHI_CAROUSEL_LIST_H_
#define _PHI_CAROUSEL_LIST_H_

#include <rendering\quadRenderer2D.h>

#include "control.h"
#include "carouselItem.h"
#include "carouselItemEventArgs.h"
#include "carouselTab.h"
#include "toggleButton.h"

#include <vector>

namespace phi
{
    class carouselList : public control
    {
    private:
        static const unsigned int HEADER_HEIGHT = 30;
        static const unsigned int SUB_TAB_WIDTH = 120;
        static const unsigned int TAB_MARGIN = 2;
        static const unsigned int REGIONS_MARGIN = 4;
        static const unsigned int EXPAND_BUTTON_WIDTH = 60;
        static const unsigned int TABS_WIDTH = HEADER_HEIGHT * 3 + TAB_MARGIN * 2;

        color _backgroundColor;
        bool _expanded;
        button* _expandButton;
        float _contentOffset;
        std::vector<std::vector<carouselTab*>*> _tabs;
        std::vector<toggleButton*> _tabsButtons;
        std::vector<std::vector<toggleButton*>*> _subTabsButtons;
        unsigned int _currentTab;
        carouselTab* _currentSubTab;
        eventHandler<carouselItemEventArgs>* _selectedItemChanged;
        texture* _arrowUpTexture;
        texture* _arrowDownTexture;
        float _tabsScrollOffset;
        float _tabsTargetScrollOffset;
        float _subTabsScrollOffset;
        float _subTabsTargetScrollOffset;
        scissor _tabsScissor;
        scissor _subTabsScissor;

    private:
        void expandButtonClick(phi::mouseEventArgs* e);
        void setContentOffset(float value);
        void notifySelectedItemChanged(carouselItemEventArgs e);
        void carouselTabSelectedItemChanged(carouselItemEventArgs e);
        void updateTabs();
        void updateScissors();
        void tabButtonCheckedChanging(controlCancelEventArgs* e);
        void subTabButtonCheckedChanging(controlCancelEventArgs* e);
        void subTabButtonCheckedChanged(controlEventArgs e);
        toggleButton* createTabButton(std::string text, texture* image, int x, int y, sizef size);
        void changeCurrentSubTab(carouselTab* subTab);

    public:
        UI_API carouselList(sizef viewportSize);
        UI_API ~carouselList();

        UI_API bool getExpanded() { return _expanded; }
        UI_API eventHandler<carouselItemEventArgs>* getSelectedItemChanged() { return _selectedItemChanged; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(sizef value) override;
        UI_API void setViewportSize(sizef value) override;
        UI_API void setBackgroundColor(color value);
        UI_API void setExpanded(bool value);

        UI_API int addTab(std::string toolTipText, texture* image);
        UI_API void addSubTab(int tabIndex, carouselTab* carouselTab);

        UI_API void onMouseWheel(mouseEventArgs* e) override;

        void onRender() override;
    };
}

#endif