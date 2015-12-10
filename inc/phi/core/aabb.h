#ifndef _PHI_AABB_H_
#define _PHI_AABB_H_

#include "core.h"
#include "globals.h"
#include "size.h"

#include <vector>

namespace phi
{
    class aabb
    {
    private:
        vec3 _min;
        vec3 _max;
        vec3 _center;
        float _width;
        float _height;
        float _depth;
        float _halfWidth;
        float _halfHeight;
        float _halfDepth;
        float _radius;

    public:

        CORE_API aabb(){}
        CORE_API aabb(vec3 position, sizef size);
        CORE_API aabb(vec3 min, vec3 max);
        CORE_API aabb(std::vector<vec3> points);
        CORE_API ~aabb();

        CORE_API vec3 getMin() const { return _min; }
        CORE_API vec3 getMax() const { return _max; }
        CORE_API vec3 getCenter() const { return _center; }
        CORE_API float getRadius() const { return _radius; }

		CORE_API void setMin(vec3 min) { _min = min; }
        CORE_API void setMax(vec3 max) { _max = max; }
        CORE_API void setCenter(vec3 center) { _center = center; }
        CORE_API void setRadius(float radius) { _radius = radius; }

        CORE_API float getWidth() { return _width; }
        CORE_API float getHeight() { return _height; }
        CORE_API float getDepth() { return _depth; }
        CORE_API float getHalfWidth() { return _halfWidth; }
        CORE_API float getHalfHeight() { return _halfHeight; }
        CORE_API float getHalfDepth() { return _halfDepth; }

        CORE_API bool contains(vec3 position);

        CORE_API void update(std::vector<vec3> &points);
    };
}

#endif