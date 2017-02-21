#pragma once
#include <phi.h>
#include "coreApi.h"
#include <core\component.h>
#include <core\image.h>

namespace phi
{
    struct skyBoxImages
    {
        image* PositiveX;
        image* NegativeX;
        image* PositiveY;
        image* NegativeY;
        image* PositiveZ;
        image* NegativeZ;
    };

    class skyBox :
        public component
    {
    public:
        static componentType getComponentType()
        {
            return componentType::SKY_BOX;
        }

    public:
        CORE_API skyBox(const skyBoxImages& images);
        CORE_API skyBox(const skyBox& original);
        CORE_API ~skyBox();

        CORE_API component* clone() const override;
        skyBoxImages getImages() const { return _images; }

    private:
        skyBoxImages _images;
    };
}