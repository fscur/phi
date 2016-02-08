#ifndef _PHI_MESH_H_
#define _PHI_MESH_H_

#include <phi\core\geometry.h>
#include <phi\core\object3D.h>

#include "material.h"

namespace phi
{
    class mesh : 
        public object3D
    {
    public:
        phi::geometry* geometry;
        phi::material* material;
    public:
        RENDERING_API mesh(std::string name, phi::geometry* geometry, phi::material* material) :
            object3D(name, objectType::MESH), 
            geometry(geometry), 
            material(material) {}
    };
}
#endif