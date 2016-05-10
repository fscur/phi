#include <precompiled.h>
#include "octree.h"

namespace phi
{
    octree::octree(aabb aabb, uint maxLevels, uint totalItems) :
        _children(),
        _parent(nullptr),
        _index(0),
        _indexPtr(&_index),
        _maxLevels(maxLevels),
        _maxItems(0),
        _currentLevel(0),
        _items(map<uint, vector<positionData>>()),
        _positions(vector<vec3>()),
        _aabb(aabb),
        _isSubdivided(false)
    {
        _maxItems = static_cast<uint>(totalItems / static_cast<uint>(glm::pow<float>(8.0f, static_cast<float>(_maxLevels))));

        if (_maxItems < 10)
            _maxItems = 10;
    }

    octree::octree(octree* parent, uint index, uint maxItems)
    {
        _indexPtr = parent->_indexPtr;
        _currentLevel = parent->_currentLevel + 1;
        _parent = parent;
        _aabb = createAabb(parent->_aabb, index);
        _maxItems = maxItems;
    }

    aabb octree::createAabb(aabb parentAabb, uint index)
    {
        auto width = parentAabb.halfWidth;
        auto height = parentAabb.halfHeight;
        auto depth = parentAabb.halfDepth;

        auto minX = 0.0f;
        auto minY = 0.0f;
        auto minZ = 0.0f;
        auto maxX = 0.0f;
        auto maxY = 0.0f;
        auto maxZ = 0.0f;

        auto parentAabbMin = parentAabb.min;
        auto parentAabbMax = parentAabb.max;

        auto xIndex = index & 4;
        auto yIndex = index & 2;
        auto zIndex = index & 1;

        xIndex = xIndex >> 2;
        yIndex = yIndex >> 1;

        if (xIndex == 0)
        {
            minX = parentAabbMin.x;
            maxX = parentAabbMin.x + width;
        }
        else
        {
            minX = parentAabbMin.x + width;
            maxX = parentAabbMax.x;
        }

        if (yIndex == 0)
        {
            minY = parentAabbMin.y;
            maxY = parentAabbMin.y + height;
        }
        else
        {
            minY = parentAabbMin.y + height;
            maxY = parentAabbMax.y;
        }

        if (zIndex == 0)
        {
            minZ = parentAabbMin.z;
            maxZ = parentAabbMin.z + depth;
        }
        else
        {
            minZ = parentAabbMin.z + depth;
            maxZ = parentAabbMax.z;
        }

        auto min = glm::vec3(minX, minY, minZ);
        auto max = glm::vec3(maxX, maxY, maxZ);

        return phi::aabb(min, max);
    }

    void octree::insertIntoChildren(vec3 position, vector<positionData> posDataList)
    {
        auto pos = position;

        pos -= _aabb.min;

        auto x = (int)(pos.x / _aabb.halfWidth);
        auto y = (int)(pos.y / _aabb.halfHeight);
        auto z = (int)(pos.z / _aabb.halfDepth);

        auto childOctreeIndex = 0;

        childOctreeIndex |= x;
        childOctreeIndex = childOctreeIndex << 1;
        childOctreeIndex |= y;
        childOctreeIndex = childOctreeIndex << 1;
        childOctreeIndex |= z;

        //auto childOctree = _children[childOctreeIndex];

        /*for (unsigned int i = 0; i < 8; i++)
        {
        auto childOctree = _children[i];

        if (childOctree->_aabb.contains(position))
        {
        childOctree->_positions.push_back(position);
        childOctree->_items[childOctree->_positions.size() - 1] = posDataList;
        break;
        }
        }*/
    }

    bool octree::insertIntoChildren(vertex vertex, uint &index)
    {
        auto pos = vertex.position;

        pos -= _aabb.min;

        auto x = (int)(pos.x / _aabb.halfWidth);
        auto y = (int)(pos.y / _aabb.halfHeight);
        auto z = (int)(pos.z / _aabb.halfDepth);

        auto childOctreeIndex = 0;

        childOctreeIndex |= x;
        childOctreeIndex = childOctreeIndex << 1;
        childOctreeIndex |= y;
        childOctreeIndex = childOctreeIndex << 1;
        childOctreeIndex |= z;

        auto childOctree = _children[childOctreeIndex];

        return childOctree->insert(vertex, index);
    }

    void octree::subdivide()
    {
        for (auto i = 0; i < 8; i++)
            _children[i] = new octree(this, i, _maxItems);

        auto positionsCount = static_cast<int>(_positions.size());
        for (auto i = 0; i < positionsCount; i++)
        {
            insertIntoChildren(_positions[i], _items[i]);
        }

        _items.clear();
        _positions.clear();
        _isSubdivided = true;
    }

    bool octree::insert(phi::vertex vertex, uint &index)
    {
        auto pos = vertex.position;

        if (!_isSubdivided)
        {
            auto s = _positions.size();

            if (s >= _maxItems)
            {
                subdivide();
                return insertIntoChildren(vertex, index);
            }

            bool found = false;
            unsigned int posIndex = 0;

            for (unsigned int i = 0; i < s; i++)
            {
                if (_positions[i] == pos)
                {
                    found = true;
                    posIndex = i;
                    break;
                }
            }

            if (!found)
            {
                auto posData = positionData();

                posData.index = (*_indexPtr)++;
                posData.vertex = vertex;

                std::vector<positionData> posDataList;
                posDataList.push_back(posData);

                _positions.push_back(pos);
                auto pSize = (unsigned int)_positions.size();
                _items[pSize - 1] = posDataList;

                index = posData.index;

                return true;
            }
            else if (found)
            {
                auto posDataList = &(_items[posIndex]);
                auto s = posDataList->size();

                for (unsigned int i = 0; i < s; i++)
                {
                    auto pData = (*posDataList)[i];
                    if (pData.vertex == vertex)
                    {
                        index = pData.index;
                        return false;
                    }
                }

                auto posData = positionData();

                posData.index = (*_indexPtr)++;
                posData.vertex = vertex;

                posDataList->push_back(posData);

                index = posData.index;
                return true;
            }

            return false;
        }
        else
        {
            return insertIntoChildren(vertex, index);
        }

        return false;
    }
}