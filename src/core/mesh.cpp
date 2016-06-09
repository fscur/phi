#include <precompiled.h>
#include "mesh.h"

namespace phi
{
    mesh::mesh(string name, phi::geometry* geometry, phi::material* material) :
        component(componentType::MESH, name),

        _id(-1),
        _geometry(geometry),
        _material(material)
    {
    }

    mesh::~mesh()
    {
    }

    mesh::mesh(const mesh& mesh) :
        component(componentType::MESH, mesh._name),
        _id(-1),
        _geometry(mesh._geometry),
        _material(mesh._material)
    {
    }

    component* mesh::clone() const
    {
        auto m = static_cast<const mesh*>(this);
        return new mesh(*m);
    }

    vec3 mesh::getSelectionColor()
    {
        auto r = static_cast<float>(_id & 255);
        auto g = static_cast<float>((_id >> 8) & 255);
        auto b = static_cast<float>((_id >> 16) & 255);

        return vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    }
}