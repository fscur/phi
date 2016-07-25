#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\image.h>

namespace phi
{
    class planeGrid :
        public component
    {
    private:
        image* _image;
    public:
        static componentType getComponentType() 
        { 
            return componentType::PLANE_GRID; 
        }

    public:
        UI_API planeGrid();
        UI_API planeGrid(const planeGrid& planeGrid);
        UI_API ~planeGrid();

        void setImage(image* value) { _image = value; }
        image* getImage() const { return _image; }

        UI_API component* clone() const override;
    };
}