#pragma once

#include <core\geometry.h>
#include <core\object3D.h>

#include "material.h"

namespace phi
{
    class mesh :
        public object3D
    {
    public:
        phi::geometry* geometry;
        phi::material* material;

    //protected:
        /*RENDERING_API mesh(const mesh& original) : object3D(original)
        {
            geometry = original.geometry;
            material = original.material;
        }*/

    public:

        RENDERING_API mesh(std::string name, phi::geometry* geometry, phi::material* material) :
            object3D(name, objectType::MESH),
            geometry(geometry),
            material(material){}

        RENDERING_API object3D* clone() override
        {
            auto m = dynamic_cast<mesh*>(this);
            return new mesh(*m);
        }
    };
}