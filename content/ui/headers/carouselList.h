#ifndef _PHI_CAROUSEL_LIST_H_
#define _PHI_CAROUSEL_LIST_H_

#include <vector>
#include "control.h"
#include "carouselItem.h"
#include "carouselItemEventArgs.h"
#include "carouselTab.h"
#include "toggleButton.h"
#include "quadRenderer2D.h"

namespace phi
{
    class carouselList : public control
    {
    private:
        static const unsigned int BUTTON_MARGIN = 30;

        color _backgroundColor;
        bool _expanded;
        button* _expandButton;
        float _contentOffset;
        std::vector<carouselTab*> _tabs;
        std::vector<toggleButton*> _tabsButtons;
        carouselTab* _currentTab;
        eventHandler<carouselItemEventArgs>* _selectedItemChanged;
        texture* _arrowUpTexture;
        texture* _arrowDownTexture;

    private:
        void expandButtonClick(phi::mouseEventArgs* e);
        void setContentOffset(float value);
        void notifySelectedItemChanged(carouselItemEventArgs e);
        void carouselTabSelectedItemChanged(carouselItemEventArgs e);
        void updateTabs();
        void tabButtonCheckedChanging(controlCancelEventArgs* e);

    public:
        UI_API carouselList(size<GLuint> viewportSize);
        UI_API ~carouselList();

        UI_API bool getExpanded();
        UI_API eventHandler<carouselItemEventArgs>* getSelectedItemChanged() { return _selectedItemChanged; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;
        UI_API void setBackgroundColor(color value);
        UI_API void setExpanded(bool value);

        UI_API void addTab(carouselTab* carouselTab);

        void update() override;
        void onRender() override;
    };
}

#endif