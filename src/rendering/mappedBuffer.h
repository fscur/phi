#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\exception.h>

#include "buffer.h"
#include "bufferSlot.h"

#include <GL\glew.h>

namespace phi
{
    //TODO: create option to be mutable or immutable (see also multiDrawMappedBuffer)
    template <typename KEY, typename DATA>
    class mappedBuffer :
        public buffer
    {
    public:
        mappedBuffer(string name, bufferTarget::bufferTarget target) :
            buffer(name, target),
            _sizeOfDataType(sizeof(DATA)),
            _instances(unordered_map<KEY, bufferSlot>()),
            _instanceCount(0),
            _capacity(0)
        {
        }

        ~mappedBuffer() {}

        void add(KEY key, DATA data)
        {
            auto index = _instanceCount++;

            _instances[key] = bufferSlot(index, 1);
            _bufferData.insert(_bufferData.begin() + index, data);

            uploadData(index, 1);
        }

        void addRange(KEY key, DATA* data, size_t size)
        {
            auto index = _instanceCount;
            _instances[key] = bufferSlot(index, size);
            _instanceCount += size;

            _bufferData.insert(_bufferData.begin() + index, data, data + size);
            uploadData(index, size);
        }

        void update(KEY key, DATA data)
        {
            assert(_instances.find(key) != _instances.end());

            auto index = _instances[key].index;
            _bufferData[index] = data;
            uploadData(index, 1);
        }

        void updateRange(KEY key, DATA* data, size_t size)
        {
            auto bufferSlot = _instances[key];

            if (bufferSlot.size == size)
            {
                auto index = _instances[key].index;

                std::copy(data, data + size, _bufferData.begin() + index);
                uploadData(index, size);
            }
            else
            {
                remove(key);
                addRange(key, data, size);
            }
        }

        void remove(KEY key)
        {
            assert(_instances.find(key) != _instances.end());

            auto index = _instances[key].index;
            auto size = _instances[key].size;
            auto dataCount = _instanceCount - (index + size);

            auto first = _bufferData.begin() + index;
            auto last = first + size;
            _bufferData.erase(first, last);

            uploadData(index, dataCount);

            for (auto& pair : _instances)
            {
                if (pair.second.index < index)
                    continue;

                pair.second.index -= size;
            }

            _instanceCount -= size;
            _instances.erase(key);
        }

        size_t getInstanceCount() const { return _instanceCount; }

    private:
        void uploadData(size_t index, size_t size)
        {
            auto needToResize = _capacity != _bufferData.capacity();
            if (needToResize)
            {
                _capacity = _bufferData.capacity();
                data(_sizeOfDataType * _capacity, _bufferData.data(), bufferDataUsage::dynamicDraw);
            }
            else if (size > 0)
            {
                subData(_sizeOfDataType * index, _sizeOfDataType * size, &_bufferData[index]);
            }
        }

    private:
        unsigned long long _sizeOfDataType;
        unordered_map<KEY, bufferSlot> _instances;
        vector<DATA> _bufferData;
        size_t _instanceCount;
        size_t _capacity;
    };
}
