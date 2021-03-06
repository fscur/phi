#pragma once

#include <phi.h>

#include "vertex.h"
#include "aabb.h"

namespace phi
{
    class octree
    {
    private:
        struct positionData
        {
            uint index;
            phi::vertex vertex;
        };

    public:
        octree(aabb aabb, uint maxLevels, uint totalItems);
        octree(octree* parent, uint index, uint maxItems);
        ~octree();
        bool insert(vertex vertex, int &index);

    private:
        bool insertIntoChildren(vertex vertex, int &index);
        void insertIntoChildren(vec3 position, vector<positionData> posDataList);
        aabb createAabb(aabb parentAabb, uint index);
        void subdivide();

    private:
        octree* _children[8];
        octree* _parent;
        uint _index;
        uint* _indexPtr;
        uint _maxLevels;
        uint _maxItems;
        uint _currentLevel;
        map<uint, vector<positionData>> _items;
        vector<vec3> _positions;
        aabb _aabb;
        bool _isSubdivided;
    };
}
