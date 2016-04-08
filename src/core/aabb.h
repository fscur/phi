#pragma once
#include <phi.h>
#include "coreApi.h"

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
        CORE_API aabb(vec3 min, vec3 max);
        CORE_API aabb(const vector<vec3> &points);

        CORE_API bool contains(vec3 position) const;
        CORE_API void update(const vector<vec3> &points);

        vec3 getMin() const { return _min; }
        vec3 getMax() const { return _max; }
        vec3 getCenter() const { return _center; }
        float getRadius() const { return _radius; }

        void setMin(vec3 min) { _min = min; }
        void setMax(vec3 max) { _max = max; }
        void setCenter(vec3 center) { _center = center; }
        void setRadius(float radius) { _radius = radius; }

        float getWidth() const { return _width; }
        float getHeight() const { return _height; }
        float getDepth() const { return _depth; }
        float getHalfWidth() const { return _halfWidth; }
        float getHalfHeight() const { return _halfHeight; }
        float getHalfDepth() const { return _halfDepth; }

        
    };
}