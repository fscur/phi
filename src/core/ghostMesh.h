#pragma once
#include <phi.h>
#include "coreApi.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\component.h>

#include "material.h"

namespace phi
{
    class ghostMesh :
        public component
    {
    private:
        geometry* _geometry;
        material* _material;
        vec3 _offset;

    public:
        CORE_API ghostMesh(phi::geometry* geometry, phi::material* material);
        CORE_API ghostMesh(const ghostMesh& mesh);
        CORE_API ~ghostMesh();

        CORE_API component* clone() const override;

        material* getMaterial() const { return _material; }
        geometry* getGeometry() const { return _geometry; }
        mat4 getModelMatrix() const { return getNode()->getTransform()->getModelMatrix(); }

        static componentType getComponentType() { return componentType::GHOST_MESH; }
    };
}