#ifndef _PHI_RAY_H_
#define _PHI_RAY_H_

#include "rendering.h"
#include "aabb.h"

namespace phi
{
    class ray
    {
    private:
        glm::vec3 _origin;
        glm::vec3 _direction;
    public:
        RENDERING_API ray(glm::vec3 origin = glm::vec3(), glm::vec3 direction = glm::vec3());
        RENDERING_API ~ray();

        RENDERING_API glm::vec3 getOrigin() const { return _origin; }
        RENDERING_API glm::vec3 getDirection() const { return _direction; }

        RENDERING_API bool intersects(aabb* aabb);
        RENDERING_API bool intersects(aabb* aabb, glm::vec3* normal);
        RENDERING_API bool intersects(glm::vec3 bl, glm::vec3 tl, glm::vec3 tr, glm::vec3 br, float* t);
    };
}
#endif