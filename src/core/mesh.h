#pragma once
#include <phi.h>
#include "coreApi.h"

#include <core\geometry.h>
#include <core\component.h>
#include "material.h"

namespace phi
{
    class mesh :
        public component
    {
    private:
        int _id;

    public:
        geometry* geometry;
        material* material;

    public:
        CORE_API mesh(string name, phi::geometry* geometry, phi::material* material);
        CORE_API mesh(const mesh& mesh);
        CORE_API ~mesh();

        CORE_API component* clone() const override;
        CORE_API vec3 getSelectionColor();
        
        void setId(int id) { _id = id; }
        int getId() const { return _id; }
        static componentType getComponentType() { return componentType::MESH; }
    };
}