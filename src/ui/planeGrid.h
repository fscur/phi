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

        UI_API void setColor(const color& value) { _color = value; }
        UI_API void setLineThickness(const float value) { _lineThickness = value; }

        UI_API component* clone() const override;
    };
}