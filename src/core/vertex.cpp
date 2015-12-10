#include <phi/core/vertex.h>

namespace phi
{
    int vertex::SIZE = 11;

    vertex::vertex()
    {
        _position = vec3(0.0f);
        _texCoord = vec2(0.0f);
        _normal = vec3(0.0f);
        _tangent = vec3(0.0f);
        _bitangent = vec3(0.0);
    }

    vertex::vertex(vec3 position, vec2 texCoord, vec3 normal, vec3 tangent)
    {
        _position = position;
        _texCoord = texCoord;
        _normal = normal;
        _tangent = tangent;
        _bitangent = vec3(0.0);
    }

    vertex::~vertex()
    {}
}