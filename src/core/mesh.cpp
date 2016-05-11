#include <precompiled.h>
#include "mesh.h"

namespace phi
{
    mesh::mesh(string name, phi::geometry* geometry, phi::material* material) :
        component(componentType::MESH, name),
        _selectionChanged(new eventHandler<mesh*>()),
        _isSelected(false),
        _id(-1),
        geometry(geometry),
        material(material)
    {
    }

    mesh::~mesh()
    {
        safeDelete(_selectionChanged);
    }

    mesh::mesh(const mesh& mesh) :
        component(componentType::MESH, mesh._name),
        _selectionChanged(new eventHandler<phi::mesh*>()),
        _isSelected(false),
        _id(-1),
        geometry(mesh.geometry),
        material(mesh.material)
    {
    }

    component* mesh::clone() const
    {
        auto m = static_cast<const mesh*>(this);
        return new mesh(*m);
    }

    void mesh::setSelected(bool isSelected)
    {
        _isSelected = isSelected;
        _selectionChanged->raise(this);
    }

    vec4 mesh::getSelectionColor()
    {
        auto r = static_cast<float>(_id & 255);
        auto g = static_cast<float>((_id >> 8) & 255);
        auto b = static_cast<float>((_id >> 16) & 255);
        auto a = _isSelected;

        return vec4(r / 255.0f, g / 255.0f, b / 255.0f, a);
    }
}