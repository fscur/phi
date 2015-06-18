#ifndef _PHI_AABB_H_
#define _PHI_AABB_H_

#include "core.h"
#include "size.h"
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
        CORE_API aabb(){}
        CORE_API aabb(glm::vec3 position, size<float> size);
        CORE_API aabb(glm::vec3 min, glm::vec3 max);
        CORE_API aabb(std::vector<glm::vec3> points);
        CORE_API ~aabb();

        CORE_API glm::vec3& getMin() { return _min; }
        CORE_API glm::vec3& getMax() { return _max; }
        CORE_API glm::vec3& getCenter() { return _center; }
        CORE_API float& getRadius() { return _radius; }

		CORE_API void setMin(glm::vec3 min) { _min = min; }
        CORE_API void setMax(glm::vec3 max) { _max = max; }
        CORE_API void setCenter(glm::vec3 center) { _center = center; }
        CORE_API void setRadius(float radius) { _radius = radius; }

        CORE_API void update(std::vector<glm::vec3> &points);
    };
}

#endif