#ifndef _PHI_CAROUSEL_ITEM_H_
#define _PHI_CAROUSEL_ITEM_H_

#include "control.h"
#include "quadRenderer2D.h"

namespace phi
{
    class carouselItem : public control
    {
    private:
        static const float SELECTED_SCALE_PERCENT;

        texture* _texture;
        texture* _selectedTexture;
        quadRenderer2D* _backgroundRenderer;
        quadRenderer2D* _selectedRenderer;
        bool _isSelected;

    public:
        UI_API carouselItem(size<GLuint> viewportSize);
        UI_API ~carouselItem();

        UI_API bool getIsSelected() { return _isSelected; }

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setZIndex(float value) override;
        UI_API void setSize(size<GLuint> value) override;
        UI_API void setViewportSize(size<GLuint> value) override;
        UI_API void setTexture(texture* value) { _texture = value; }
        UI_API void setIsSelected(bool value);

        UI_API void render() override;
    };
}

#endif