#ifndef _PHI_MESH_H_
#define _PHI_MESH_H_

#include "geometry.h"
#include "material.h"

#include <phi/core/object3D.h>

namespace phi
{
    class mesh : 
        public object3D
    {
    private:
        geometry* _geometry;
        material* _material;
    public:
        RENDERING_API mesh(std::string name, geometry* geometry, material* material);
        RENDERING_API geometry* getGeometry() const { return _geometry; }
        RENDERING_API material* getMaterial() const { return _material; }
    };
}
#endif