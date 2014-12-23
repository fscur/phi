#include "vertex.h"

namespace phi
{
    int vertex::SIZE = 11;

    vertex::vertex()
    {
        _position = glm::vec3(0.0f);
        _texCoord = glm::vec2(0.0f);
        _normal = glm::vec3(0.0f);
		_tangent = glm::vec3(0.0f);
        _bitangent = glm::vec3(0.0);
    }

    vertex::vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal, glm::vec3 tangent)
    {
        _position = position;
        _texCoord = texCoord;
        _normal = normal;
		_tangent = tangent;
        _bitangent = glm::vec3(0.0);
    }

    vertex::~vertex()
    {}
}