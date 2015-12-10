#include <phi/core/aabb.h>

#include <algorithm>

namespace phi
{
    aabb::aabb(vec3 min, vec3 max) :
        _min(min), 
        _max(max), 
        _width(max.x - min.x),
        _height(max.y - min.y),
        _depth(max.z - min.z),
        _halfWidth((max.x - min.x) * 0.5f),
        _halfHeight((max.y - min.y) * 0.5f),
        _halfDepth((max.z - min.z) * 0.5f)
    {
        /*_min = min;
        _max = max; 
        _width = _max.x - _min.x;
        _height = _max.y - _min.y;
        _depth = _max.z - _min.z;
        _halfWidth = _width * 0.5f;
        _halfHeight = _height * 0.5f;
        _halfDepth = _depth * 0.5f;*/
        _center = (_min + _max) / 2.0f;
        _radius = length(_min - _center);
    }

    aabb::aabb(vec3 position, sizef size)
    {
        vec3 halfSize = vec3(size.w, size.h, size.d) / 2.0f;
        _min = position - halfSize;
        _max = position + halfSize;
        _width = _max.x - _min.x;
        _height = _max.y - _min.y;
        _depth = _max.z - _min.z;
        _halfWidth = _width * 0.5f;
        _halfHeight = _height * 0.5f;
        _halfDepth = _depth * 0.5f;
        _center = (_min + _max) / 2.0f;
        _radius = length(_min - _center);
    }

    aabb::aabb(std::vector<vec3> points)
    {
        update(points);
    }

    aabb::~aabb(){}

    bool aabb::contains(vec3 pos)
    {
        return
            _min.x <= pos.x && pos.x < _max.x &&
            _min.y <= pos.y && pos.y < _max.y &&
            _min.z <= pos.z && pos.z < _max.z;
    }

    void aabb::update(std::vector<vec3> &points)
    {
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        float maxZ = std::numeric_limits<float>::lowest();

        for(auto pos : points)
        {
            if (pos.x < minX)
                minX = pos.x;

            if (pos.y < minY)
                minY = pos.y;

            if (pos.z < minZ)
                minZ = pos.z;

            if (pos.x > maxX)
                maxX = pos.x;

            if (pos.y > maxY)
                maxY = pos.y;

            if (pos.z > maxZ)
                maxZ = pos.z;
        }

        _min = vec3(minX, minY, minZ);
        _max = vec3(maxX, maxY, maxZ);
        _width = _max.x - _min.x;
        _height = _max.y - _min.y;
        _depth = _max.z - _min.z;
        _halfWidth = _width * 0.5f;
        _halfHeight = _height * 0.5f;
        _halfDepth = _depth * 0.5f;
        _center = (_min + _max) / 2.0f;
        _radius = length(_min - _center);
    }
}