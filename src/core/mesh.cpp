#include <precompiled.h>
#include "mesh.h"

namespace phi
{
    mesh::mesh(string name, phi::geometry* geometry, phi::material* material) :
        component(componentType::MESH, name),
        _geometry(geometry),
        _material(material)
    {
    }

    mesh::~mesh()
    {
    }

    mesh::mesh(const mesh& mesh) :
        component(componentType::MESH, mesh._name),
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