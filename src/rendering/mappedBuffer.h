#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\exception.h>

#include "buffer.h"
#include "bufferSlot.h"

#include <GL\glew.h>

namespace phi
{
    template <typename KEY, typename DATA>
    class mappedBuffer :
        public buffer
    {

    private:
        size_t _maxInstances;
        unsigned long long _sizeOfDataType;
        unordered_map<KEY, bufferSlot> _instances;
        DATA* _bufferData;
        size_t _instanceCount;

    public:
        mappedBuffer(string name, bufferTarget::bufferTarget target, size_t maxInstances, bool data = false) :
            buffer(name, target),
            _maxInstances(maxInstances),
            _sizeOfDataType(sizeof(DATA)),
            _instances(unordered_map<KEY, bufferSlot>()),
            _bufferData(new DATA[_maxInstances]),
            _instanceCount(0)
        {
            if (data)//TODO: remover this shit
                this->data(sizeof(uint), nullptr, bufferDataUsage::dynamicDraw);
            else
                storage(maxInstances * _sizeOfDataType, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);
        }

        void add(KEY key, DATA data)
        {
            auto index = _instanceCount;

            _instances[key] = bufferSlot(index, 1);
            _instanceCount++;
            _bufferData[index] = data;
            subData(index * _sizeOfDataType, _sizeOfDataType, &data);
        }

        void addRange(KEY key, DATA* data, size_t size)
        {
            auto index = _instanceCount;
            _instances[key] = bufferSlot(index, size);
            _instanceCount += size;

            auto bufferSize = size * _sizeOfDataType;
            memcpy(_bufferData + index, data, bufferSize);
            subData(index * _sizeOfDataType, bufferSize, data);
        }

        void update(KEY key, DATA data)
        {
            auto index = _instances[key].index;
            _bufferData[index] = data;
            subData(index * _sizeOfDataType, _sizeOfDataType, &data);
        }

        void updateRange(KEY key, DATA* data, size_t size)
        {
            auto bufferSlot = _instances[key];

            if (bufferSlot.size == size)
            {
                auto index = _instances[key].index;
                auto bufferSize = size * _sizeOfDataType;
                memcpy(_bufferData + index, data, bufferSize);
                subData(index * _sizeOfDataType, bufferSize, data);
            }
            else
            {
                remove(key);
                addRange(key, data, size);
            }
        }

        void remove(KEY key)
        {
            auto index = _instances[key].index;
            auto size = _instances[key].size;
            auto dataCount = _instanceCount - (index + size);

            if (dataCount > 0)
            {
                auto auxBuffer = new DATA[dataCount];
                auto sizeOfDataToRemove = dataCount * _sizeOfDataType;
                auto keyDataOffset = _bufferData + index;

                memcpy(auxBuffer, keyDataOffset + size, sizeOfDataToRemove);
                memcpy(keyDataOffset, auxBuffer, sizeOfDataToRemove);

                subData(index * _sizeOfDataType, sizeOfDataToRemove, keyDataOffset);
                safeDeleteArray(auxBuffer);
            }

            for (auto& pair : _instances)
            {
                if (pair.second.index < index)
                    continue;

                pair.second.index-= size;
            }

            _instanceCount -= size;
            _instances.erase(key);
        }

        size_t getInstanceCount() const { return _instanceCount; }
    };
}
