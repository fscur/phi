#include <precompiled.h>
#include "mesh.h"

namespace phi
{
    mesh::mesh(phi::geometry* geometry, phi::material* material) :
        component(componentType::MESH),
        _geometry(geometry),
        _material(material)
    {
    }

    mesh::~mesh()
    {
    }

    mesh::mesh(const mesh& mesh) :
        component(componentType::MESH),
        _geometry(mesh._geometry),
        _material(mesh._material)
    {
    }

    component* mesh::clone() const
    {
        auto m = static_cast<const mesh*>(this);
        return new mesh(*m);
    }
}