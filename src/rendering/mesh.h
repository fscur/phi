#pragma once

#include <core\geometry.h>
#include <core\component.h>

#include "material.h"

namespace phi
{
    class mesh :
        public component
    {
    public:
        phi::geometry* geometry;
        phi::material* material;

    public:

        RENDERING_API mesh(std::string name, phi::geometry* geometry, phi::material* material) :
            component(componentType::MESH, name),
            geometry(geometry),
            material(material) {}

        RENDERING_API component* clone() override
        {
            auto m = dynamic_cast<mesh*>(this);
            return new mesh(*m);
        }
    };
}