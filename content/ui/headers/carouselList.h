#ifndef _PHI_CAROUSEL_LIST_H_
#define _PHI_CAROUSEL_LIST_H_

#include <vector>
#include "control.h"
#include "carouselItem.h"
#include "quadRenderer2D.h"

namespace phi
{
    class carouselList : public control
    {
    private:
        static const unsigned int ITEM_MARGIN = 10;

        texture* _texture;
        quadRenderer2D* _backgroundRenderer;
        std::vector<carouselItem*> _items;
        color _backgroundColor;
        float _scrollOffset;
        float _targetScrollOffset;

    private:
        void updateItems();

    public:
        UI_API carouselList(size<GLuint> viewportSize);
        UI_API ~carouselList();

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;

        UI_API void onMouseDown(mouseEventArgs* e) override;
        UI_API void onMouseWheel(mouseEventArgs* e) override;

        UI_API void render() override;

        UI_API void setBackgroundColor(color value);

        UI_API void addCarouselItem(carouselItem* carouselItem);
    };
}

#endif