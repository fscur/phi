#ifndef _PHI_CAROUSEL_LIST_H_
#define _PHI_CAROUSEL_LIST_H_

#include <vector>
#include "control.h"
#include "carouselItem.h"
#include "carouselItemEventArgs.h"
#include "button.h"
#include "quadRenderer2D.h"

namespace phi
{
    class carouselList : public control
    {
    private:
        static const unsigned int ITEM_MARGIN = 10;
        static const unsigned int BUTTON_MARGIN = 30;

        texture* _texture;
        quadRenderer2D* _backgroundRenderer;
        std::vector<carouselItem*> _items;
        color _backgroundColor;
        float _scrollOffset;
        float _targetScrollOffset;
        float _contentOffset;
        eventHandler<carouselItemEventArgs>* _selectedItemChanged;
        bool _expanded;
        button* _expandButton;

    private:
        void updateItems();
        void notifySelectedItemChanged(carouselItemEventArgs e);
        void carouselItemIsSelectedChanged(carouselItemEventArgs e);
        void expandButtonClick(phi::mouseEventArgs* e);
        void setContentHeight(float value);

    public:
        UI_API carouselList(size<GLuint> viewportSize);
        UI_API ~carouselList();

        UI_API std::vector<carouselItem*> getItems() { return _items; }
        UI_API eventHandler<carouselItemEventArgs>* getSelectedItemChanged() { return _selectedItemChanged; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;
        UI_API void setBackgroundColor(color value);
        UI_API void setSelectedItem(carouselItem* item);

        UI_API void onMouseMove(mouseEventArgs* e) override;
        UI_API void onMouseDown(mouseEventArgs* e) override;
        UI_API void onMouseUp(mouseEventArgs* e) override;
        UI_API void onMouseWheel(mouseEventArgs* e) override;

        UI_API void render() override;

        UI_API void addCarouselItem(carouselItem* carouselItem);
    };
}

#endif