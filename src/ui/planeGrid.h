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

        eventHandler<planeGrid*>* _visibleChanged;

    public:
        static componentType getComponentType() 
        { 
            return componentType::PLANE_GRID; 
        }

    public:
        UI_API planeGrid();
        UI_API planeGrid(const planeGrid& planeGrid);
        UI_API ~planeGrid();

        UI_API color getColor() const { return _color; }
        UI_API float getLineThickness() const { return _lineThickness; }
        UI_API float getOpacity() const { return _opacity; }
        UI_API bool getVisible() const { return _visible; }
        UI_API eventHandler<planeGrid*>* getVisibleChanged() { return _visibleChanged; }

        UI_API void setColor(const color& value) { _color = value; }
        UI_API void setLineThickness(const float value) { _lineThickness = value; }
        UI_API void setOpacity(const float value) { _opacity = value; }

        UI_API void hide();
        UI_API void show();

        UI_API component* clone() const override;
    };
}