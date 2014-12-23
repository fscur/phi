#ifndef _PHI_AABB_H_
#define _PHI_AABB_H_

#include "rendering.h"
#include "size.h"
#include "Vertex.h"
#include <vector>
#include <glm/glm.hpp>

namespace phi
{
    class aabb
    {
    private:
        glm::vec3 _min;
        glm::vec3 _max;
        glm::vec3 _center;
        float _radius;

    public:
        RENDERING_API aabb(){}
        RENDERING_API aabb(glm::vec3 position, size<float> size);
        RENDERING_API aabb(glm::vec3 min, glm::vec3 max);
        RENDERING_API aabb(std::vector<glm::vec3> points);
        RENDERING_API ~aabb();

        RENDERING_API glm::vec3& getMin() { return _min; }
        RENDERING_API glm::vec3& getMax() { return _max; }
        RENDERING_API glm::vec3& getCenter() { return _center; }
        RENDERING_API float& getRadius() { return _radius; }

		RENDERING_API void setMin(glm::vec3 min) { _min = min; }
        RENDERING_API void setMax(glm::vec3 max) { _max = max; }
        RENDERING_API void setCenter(glm::vec3 center) { _center = center; }
        RENDERING_API void setRadius(float radius) { _radius = radius; }

        RENDERING_API void update(std::vector<glm::vec3> &points);
    };
}

#endif