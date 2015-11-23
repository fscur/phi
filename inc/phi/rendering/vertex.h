#ifndef _PHI_VERTEX_H_
#define _PHI_VERTEX_H_

#include "phi/rendering/rendering.h"
#include <glm\glm.hpp>

namespace phi
{
    class vertex
    {
    private:
        glm::vec3 _position;
        glm::vec2 _texCoord;
        glm::vec3 _normal;
		glm::vec3 _tangent;
        glm::vec3 _bitangent;
    public :
        static int SIZE;
    public:
        RENDERING_API vertex();
        RENDERING_API vertex(glm::vec3 position, glm::vec2 texCoord = glm::vec2(0.0f), glm::vec3 normal = glm::vec3(0.0f), glm::vec3 tangent = glm::vec3(0.0f));
        RENDERING_API ~vertex();

        RENDERING_API glm::vec3 getPosition() { return _position; }
		RENDERING_API glm::vec2 getTexCoord() { return _texCoord; }
		RENDERING_API glm::vec3 getNormal() { return _normal; }
		RENDERING_API glm::vec3 getTangent() { return _tangent; }
        RENDERING_API glm::vec3 getBitangent() { return _bitangent; }

        RENDERING_API void setPosition(glm::vec3 position) { _position = position; }
        RENDERING_API void setTexCoord(glm::vec2 texCoord) { _texCoord = texCoord; }
        RENDERING_API void setNormal(glm::vec3 normal) { _normal = normal; }
		RENDERING_API void setTangent(glm::vec3 tangent) { _tangent = tangent; }
        RENDERING_API void setBitangent(glm::vec3 bitangent) { _bitangent = bitangent; }

        RENDERING_API friend bool operator==(const vertex& a, const vertex& b)
        {
            return 
                a._position == b._position && 
                a._texCoord == b._texCoord &&
                a._normal == b._normal &&
				a._tangent == b._tangent;
        }

        RENDERING_API friend bool operator!=(const vertex& a, const vertex& b)
        {
            return !(a == b);
        }
    };
}
#endif