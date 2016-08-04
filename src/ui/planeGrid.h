#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\color.h>

namespace phi
{
    class planeGrid :
        public component
    {
    private:
        color _color;
        float _lineThickness;
        float _opacity;
        bool _visible;

        eventHandler<planeGrid*> _colorChanged;
        eventHandler<planeGrid*> _lineThicknessChanged;
        eventHandler<planeGrid*> _opacityChanged;
        eventHandler<planeGrid*> _visibleChanged;

    public:
        static componentType getComponentType() 
        { 
            return componentType::PLANE_GRID; 
        }

    public:
        UI_API planeGrid();
        UI_API planeGrid(const planeGrid& planeGrid);
        UI_API ~planeGrid();

        color getColor() const { return _color; }
        float getLineThickness() const { return _lineThickness; }
        float getOpacity() const { return _opacity; }
        bool getVisible() const { return _visible; }
        eventHandler<planeGrid*>* getColorChanged() { return &_colorChanged; }
        eventHandler<planeGrid*>* getLineThicknessChanged() { return &_lineThicknessChanged; }
        eventHandler<planeGrid*>* getOpacityChanged() { return &_opacityChanged; }
        eventHandler<planeGrid*>* getVisibleChanged() { return &_visibleChanged; }

        UI_API void setColor(const color& value);
        UI_API void setLineThickness(const float value);
        UI_API void setOpacity(const float value);

        UI_API void hide();
        UI_API void show();

        UI_API component* clone() const override;
    };
}