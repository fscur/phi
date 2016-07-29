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
        float _size;

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
        UI_API float getSize() const { return _size; }

        UI_API void setColor(const color& value) { _color = value; }
        UI_API void setSize(const float value) { _size = value; }

        UI_API component* clone() const override;
    };
}