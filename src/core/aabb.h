#pragma once

#include "globals.h"
#include "size.h"

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

        CORE_API aabb() { }
        CORE_API aabb(vec3 position, sizef size);
        CORE_API aabb(vec3 min, vec3 max);
        CORE_API aabb(const std::vector<vec3> &points);
        CORE_API ~aabb() { };

        CORE_API inline vec3 getMin() const { return _min; }
        CORE_API inline vec3 getMax() const { return _max; }
        CORE_API inline vec3 getCenter() const { return _center; }
        CORE_API inline float getRadius() const { return _radius; }

        CORE_API inline void setMin(vec3 min) { _min = min; }
        CORE_API inline void setMax(vec3 max) { _max = max; }
        CORE_API inline void setCenter(vec3 center) { _center = center; }
        CORE_API inline void setRadius(float radius) { _radius = radius; }

        CORE_API inline float getWidth() const { return _width; }
        CORE_API inline float getHeight() const { return _height; }
        CORE_API inline float getDepth() const { return _depth; }
        CORE_API inline float getHalfWidth() const { return _halfWidth; }
        CORE_API inline float getHalfHeight() const { return _halfHeight; }
        CORE_API inline float getHalfDepth() const { return _halfDepth; }

        CORE_API bool contains(vec3 position) const;

        CORE_API void update(const std::vector<vec3> &points);
    };
}