#include <phi/rendering/mesh.h>

namespace phi
{
    mesh::mesh(std::string name, geometry* geometry, material* material) :
        _geometry(geometry), _material(material), object3D(name, objectType::MESH)
    {
    }
}