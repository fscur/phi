#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\geometry.h>
#include <core\component.h>
#include "material.h"

namespace phi
{
    class mesh :
        public component
    {
    private:
        bool _isSelected;
        int _id;

    public:
        eventHandler<mesh*> selectionChanged;
        geometry* geometry;
        material* material;

    public:
        RENDERING_API mesh(string name, phi::geometry* geometry, phi::material* material);
        RENDERING_API mesh(const mesh& mesh);
        RENDERING_API ~mesh();

        RENDERING_API component* clone() const override;
        RENDERING_API vec4 getSelectionColor();
        RENDERING_API void setSelected(bool isSelected);
        bool getSelected() { return _isSelected; }
        void setId(int id) { _id = id; }
        int getId() const { return _id; }
        static componentType getComponentType() { return componentType::MESH; }
    };
}