#include <precompiled.h>
#include "ghostMesh.h"

namespace phi
{
    ghostMesh::ghostMesh(phi::geometry* geometry, phi::material* material) :
        component(componentType::GHOST_MESH),
        _geometry(geometry),
        _material(material)
    {
    }

    ghostMesh::~ghostMesh()
    {
    }

    ghostMesh::ghostMesh(const ghostMesh& ghostMesh) :
        component(componentType::GHOST_MESH),
        _geometry(ghostMesh._geometry),
        _material(ghostMesh._material)
    {
    }

    component* ghostMesh::clone() const
    {
        auto m = static_cast<const ghostMesh*>(this);
        return new ghostMesh(*m);
    }
}