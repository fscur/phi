#ifndef _PHI_CAROUSEL_TAB_H_
#define _PHI_CAROUSEL_TAB_H_

#include "phi/ui/carouselItem.h"
#include "phi/ui/button.h"

namespace phi
{
    class carouselTab : public control
    {
    private:
        static const unsigned int ITEM_MARGIN = 10;

        std::string _name;
        texture* _image;
        texture* _backgroundTexture;
        quadRenderer2D* _backgroundRenderer;
        std::vector<carouselItem*> _items;
        color _backgroundColor;
        float _scrollOffset;
        float _targetScrollOffset;

    private:
        void updateItems();
        float getItemHeight();

    public:
        UI_API carouselTab(size<GLuint> viewportSize);
        UI_API ~carouselTab();

        UI_API std::string getName() const { return _name; }
        UI_API texture* getImage() const { return _image; }
        UI_API std::vector<carouselItem*> getItems() { return _items; }

        UI_API void setName(const std::string value) { _name = value; }
        UI_API void setImage(texture* value) { _image = value; }
        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;
        UI_API void setBackgroundColor(color value);

        UI_API void addCarouselItem(carouselItem* item);

        UI_API void onMouseWheel(mouseEventArgs* e) override;

        UI_API void onRender() override;
    };
}

#endif