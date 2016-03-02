#pragma once
#include <precompiled.h>
#include "material.h"
#include <core\geometry.h>
#include <core\component.h>

namespace phi
{
    class mesh :
        public component
    {
    public:
        phi::geometry* geometry;
        phi::material* material;

    public:

        mesh(string name, phi::geometry* geometry, phi::material* material) :
            component(componentType::MESH, name),
            geometry(geometry),
            material(material) {}

        component* clone() override
        {
            auto m = dynamic_cast<mesh*>(this);
            return new mesh(*m);
        }
    };
}