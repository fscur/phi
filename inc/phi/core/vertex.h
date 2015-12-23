#ifndef _PHI_VERTEX_H_
#define _PHI_VERTEX_H_

#include "globals.h"

namespace phi
{
    class vertex
    {
    private:
        vec3 _position;
        vec2 _texCoord;
        vec3 _normal;
        vec3 _tangent;
        vec3 _bitangent;
    public:
        static int SIZE;
    public:
        CORE_API vertex();
        CORE_API vertex(vec3 position, vec2 texCoord = vec2(0.0f), vec3 normal = vec3(0.0f), vec3 tangent = vec3(0.0f));
        CORE_API ~vertex() { };

        CORE_API inline vec3 getPosition() { return _position; }
        CORE_API inline vec2 getTexCoord() { return _texCoord; }
        CORE_API inline vec3 getNormal() { return _normal; }
        CORE_API inline vec3 getTangent() { return _tangent; }
        CORE_API inline vec3 getBitangent() { return _bitangent; }

        CORE_API inline void setPosition(vec3 position) { _position = position; }
        CORE_API inline void setTexCoord(vec2 texCoord) { _texCoord = texCoord; }
        CORE_API inline void setNormal(vec3 normal) { _normal = normal; }
        CORE_API inline void setTangent(vec3 tangent) { _tangent = tangent; }
        CORE_API inline void setBitangent(vec3 bitangent) { _bitangent = bitangent; }

        CORE_API friend bool operator==(const vertex& a, const vertex& b)
        {
            return
                a._position == b._position &&
                a._texCoord == b._texCoord &&
                a._normal == b._normal &&
                a._tangent == b._tangent;
        }

        CORE_API friend bool operator!=(const vertex& a, const vertex& b)
        {
            return !(a == b);
        }
    };
}
#endif