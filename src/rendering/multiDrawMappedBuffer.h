#pragma once
#include <phi.h>

#include <core\geometry.h>

#include "vertexBuffer.h"
#include "bufferSlot.h"

namespace phi
{
    struct geometryInstanceSlot
    {
        geometryInstanceSlot(size_t index = 0u) :
            index(index),
            size(1u)
        {
        }

        size_t index;
        size_t size;
    };

    template <typename KEY, typename DATA>
    class multiDrawMappedBuffer :
        public vertexBuffer
    {
    private:
        unsigned long long _sizeOfDataType;
        size_t _instanceCount;
        vector<DATA> _bufferData;
        size_t _currentCapacity;

        map<geometry*, geometryInstanceSlot> _geometryInstances;
        map<geometry*, vector<KEY>> _geometryKeys;
        map<KEY, size_t> _keysIndices;

    private:
        void resize()
        {
            _currentCapacity = _bufferData.capacity();
        }

        void insertAt(size_t indexToInsert, DATA data)
        {
            auto it = _bufferData.begin() + indexToInsert;
            _bufferData.insert(it, data);
        }

        size_t getIndexToInsert(geometry* geometry)
        {
            auto geometryInstanceSlot = _geometryInstances[geometry];

            auto geometryIndex = geometryInstanceSlot.index;
            auto keys = _geometryKeys[geometry];
            return geometryIndex + keys.size();
        }

    public:
        multiDrawMappedBuffer(string name, vector<vertexBufferAttribute> attribs) :
            vertexBuffer(name, attribs),
            _sizeOfDataType(sizeof(DATA)),
            _currentCapacity(0),
            _instanceCount(0)
        {
        }

        void add(geometry* geometry, KEY key, DATA data)
        {
            auto containsGeometry = phi::contains(_geometryInstances, geometry);

            size_t indexToInsert = 0;
            if (containsGeometry)
            {
                indexToInsert = getIndexToInsert(geometry);
                auto sizeOfDataToMove = (_instanceCount - indexToInsert) * _sizeOfDataType;

                insertAt(indexToInsert, data);

                auto needToResize = _currentCapacity != _bufferData.capacity();

                if (needToResize)
                {
                    resize();
                    this->data(_sizeOfDataType * _currentCapacity, _bufferData.data(), bufferDataUsage::dynamicDraw);
                }
                else
                {
                    subData(_sizeOfDataType * indexToInsert, sizeOfDataToMove + _sizeOfDataType, &_bufferData[indexToInsert]);
                }

                auto instanceIndex = _geometryInstances[geometry].size;
                _geometryInstances[geometry].size++;
                auto currentIndex = _geometryInstances[geometry].index;

                _keysIndices[key] = instanceIndex;

                for (auto& pair : _geometryInstances)
                {
                    if (pair.second.index <= currentIndex)
                        continue;

                    auto keys = _geometryKeys[pair.first];
                    for (auto& key : keys)
                    {
                        _keysIndices[key]++;
                    }

                    pair.second.index++;
                }
            }
            else
            {
                indexToInsert = _instanceCount;
                _geometryInstances[geometry] = geometryInstanceSlot(indexToInsert);
                insertAt(indexToInsert, data);

                auto needToResize = _currentCapacity != _bufferData.capacity();

                if (needToResize)
                {
                    resize();
                    this->data(_sizeOfDataType * _currentCapacity, _bufferData.data(), bufferDataUsage::dynamicDraw);
                }
                else
                {
                    subData(_sizeOfDataType * indexToInsert, _sizeOfDataType, &data);
                }
            }

            _geometryKeys[geometry].push_back(key);
            _keysIndices[key] = indexToInsert;
            _instanceCount++;
        }

        void update(KEY key, DATA data)
        {
            auto keyIndex = _keysIndices[key];
            subData(_sizeOfDataType * keyIndex, _sizeOfDataType, &data);
        }

        void remove(geometry* geometry, KEY key)
        {
            auto keys = &_geometryKeys[geometry];
            auto it = std::find(keys->begin(), keys->end(), key);

            if (it != keys->end())
                keys->erase(it);

            auto keyIndex = _keysIndices[key];

            _bufferData.erase(_bufferData.begin() + keyIndex);

            auto isLastInstance = keyIndex == _bufferData.size();
            if (!isLastInstance)
            {
                auto nextKeyIndex = keyIndex + 1;
                auto sizeOfDataToMove = (_instanceCount - nextKeyIndex) * _sizeOfDataType;
                subData(_sizeOfDataType * keyIndex, sizeOfDataToMove, &_bufferData[keyIndex]);
            }

            _instanceCount--;

            _geometryInstances[geometry].size--;
            _keysIndices.erase(key);

            auto decrementKeysIndices = [&](phi::geometry* g)
            {
                auto keys = _geometryKeys[g];
                for (auto& key : keys)
                {
                    if(_keysIndices[key] > keyIndex)
                        _keysIndices[key]--;
                }
            };

            if(!isLastInstance)
                decrementKeysIndices(geometry);

            //update indices
            for (auto& pair : _geometryInstances)
            {
                if (pair.second.index <= keyIndex)
                    continue;

                decrementKeysIndices(pair.first);
                pair.second.index--;
            }
        }
    };
}