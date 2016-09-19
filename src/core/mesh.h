#pragma once
#include <phi.h>
#include "coreApi.h"

#include <core/node.h>
#include <core/geometry.h>
#include <core/component.h>

#include "material.h"

namespace phi
{
    class mesh :
        public component
    {
    private:
        geometry* _geometry;
        material* _material;

    public:
        CORE_API mesh(phi::geometry* geometry, phi::material* material);
        CORE_API mesh(const mesh& mesh);
        CORE_API ~mesh();

        CORE_API component* clone() const override;

        material* getMaterial() const { return _material; }
        geometry* getGeometry() const { return _geometry; }
        mat4 getModelMatrix() const { return getNode()->getTransform()->getModelMatrix(); }

        static componentType getComponentType() { return componentType::MESH; }
    };
}