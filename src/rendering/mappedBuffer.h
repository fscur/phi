#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\exception.h>

#include "buffer.h"

#include <GL\glew.h>

namespace phi
{
    struct bufferGap
    {
        size_t index;
        size_t size;
        bufferGap(size_t index = 0u, size_t size = 1u) :
            index(index),
            size(size)
        {
        }
    };

    template <typename KEY, typename DATA>
    class mappedBuffer :
        public buffer
    {

    private:
        size_t _maxInstances;
        unsigned long long _sizeOfDataType;
        unordered_map<KEY, bufferGap> _instances;
        DATA* _bufferData;

    public:
        mappedBuffer(string name, bufferTarget::bufferTarget target, size_t maxInstances) :
            buffer(name, target),
            _maxInstances(maxInstances),
            _sizeOfDataType(sizeof(DATA)),
            _instances(unordered_map<KEY, bufferGap>()),
            _bufferData(new DATA[_maxInstances])
        {
            storage(maxInstances * _sizeOfDataType, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);
        }

        void add(KEY key, DATA data)
        {
            auto index = _instances.size();

            if (_maxInstances == index)
                throw exception("buffer full");

            _instances[key] = bufferGap(index, 1);
            _bufferData[index] = data;
            subData(index * _sizeOfDataType, _sizeOfDataType, &data);
        }

        void addRange(KEY key, DATA* data, size_t size)
        {
            /*for (size_t i = 0; i < size; ++i)
            {

            }*/
        }

        void update(KEY key, DATA data)
        {
            auto index = _instances[key].index;
            _bufferData[index] = data;
            subData(index * _sizeOfDataType, _sizeOfDataType, &data);
        }

        void updateRange(KEY key, DATA* data, size_t size)
        {
            /*auto index = _instances[key].index;
            _bufferData[index] = data;
            subData(index * _sizeOfDataType, _sizeOfDataType, &data);*/
        }

        void remove(KEY key)
        {
            auto index = _instances[key].index;
            auto totalInstances = _instances.size();
            auto instanceCount = totalInstances - index - 1;

            if (instanceCount > 0)
            {
                DATA* copyBuffer = new DATA[instanceCount];
                auto bufferSize = instanceCount * _sizeOfDataType;
                memcpy(copyBuffer, _bufferData + index + 1, bufferSize);
                memcpy(_bufferData + index, copyBuffer, bufferSize);
                subData(index * _sizeOfDataType, bufferSize, copyBuffer);
                safeDelete(copyBuffer);
            }

            for (auto& pair : _instances)
            {
                if (pair.second.index < index)
                    continue;

                pair.second.index--;
            }

            _instances.erase(key);
        }

        size_t getInstanceCount() const { return _instances.size(); }
    };
}
