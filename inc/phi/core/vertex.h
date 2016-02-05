#ifndef _PHI_VERTEX_H_
#define _PHI_VERTEX_H_

#include "globals.h"

namespace phi
{
    struct vertex
    {
    public:
        vec3 position;
        vec2 texCoord;
        vec3 normal;
        vec3 tangent;
    public:
        CORE_API vertex(vec3 position = vec3(0.0f), vec2 texCoord = vec2(0.0f), vec3 normal = vec3(0.0f), vec3 tangent = vec3(0.0f)) :
            position(position),
            texCoord(texCoord),
            normal(normal),
            tangent(tangent)
        {
        }

        CORE_API ~vertex() { };

        CORE_API friend bool operator==(const vertex& a, const vertex& b)
        {
            return
                a.position == b.position &&
                a.texCoord == b.texCoord &&
                a.normal == b.normal &&
                a.tangent == b.tangent;
        }

        CORE_API friend bool operator!=(const vertex& a, const vertex& b)
        {
            return !(a == b);
        }
    };
}
#endif