#pragma once
#include <phi.h>

#include <core\geometry.h>

#include "vertexBuffer.h"
#include "bufferSlot.h"

namespace phi
{
    template <typename KEY, typename DATA>
    class geometryInstanceBucket
    {
    private:
        size_t _index;
        size_t _size;

        vector<KEY> _keys;
    public:
        geometryInstanceBucket(size_t index =0) :
            _index(index),
            _size(0)
        {
        }

        size_t add(KEY key)
        {
            _keys.push_back(key);

            auto keyIndex = _size++;
            return keyIndex;
        }

        void remove(KEY key)
        {
            phi::removeIfContains(_keys, key);
            --_size;
        }

        void decrementIndex() { --_index; }

        size_t getKeyIndex(KEY key) const {return phi::indexOf(_keys, key); }
        size_t getIndex() const { return _index; }
        size_t getSize() const { return _size; }
    };

    template <typename KEY, typename DATA>
    class multiDrawMappedBuffer :
        public vertexBuffer
    {
    private:
        size_t _sizeOfDataType;
        size_t _capacity;
        vector<DATA> _bufferData;
        map<geometry*, geometryInstanceBucket<KEY, DATA>> _buckets;

    private:
        size_t getBucketIndex(const geometryInstanceBucket<KEY, DATA>* bucket)
        {
            size_t index = 0;
            for (auto& pair : _buckets)
            {
                if (pair.second.getIndex() >= bucket->getIndex())
                    continue;

                index += pair.second.getSize();
            }

            return index;
        }

        size_t getInstanceIndex(const geometryInstanceBucket<KEY, DATA>* bucket, KEY key)
        {
            auto bucketIndex = getBucketIndex(bucket);
            auto indexInBucket = bucket->getKeyIndex(key);

            return bucketIndex + indexInBucket;
        }

        void uploadData(size_t instanceIndex)
        {
            auto needToResize = _capacity != _bufferData.capacity();
            if (needToResize)
            {
                _capacity = _bufferData.capacity();
                this->data(_sizeOfDataType * _capacity, _bufferData.data(), bufferDataUsage::dynamicDraw);
            }
            else
            {
                auto sizeOfDataToMove = (_bufferData.size() - instanceIndex) * _sizeOfDataType;
                subData(_sizeOfDataType * instanceIndex, sizeOfDataToMove, _bufferData.data() + instanceIndex);
            }
        }

    public:
        multiDrawMappedBuffer(string name, vector<vertexBufferAttribute> attribs) :
            vertexBuffer(name, attribs),
            _sizeOfDataType(sizeof(DATA)),
            _capacity(0),
            _bufferData(vector<DATA>()),
            _buckets(map<geometry*, geometryInstanceBucket<KEY, DATA>>())
        {
        }

        void createBucket(geometry* geometry)
        {
            assert(!phi::contains(_buckets, geometry));

            auto nextBucketIndex = _buckets.size();
            _buckets[geometry] = geometryInstanceBucket<KEY, DATA>(nextBucketIndex);
        }

        void addInstance(geometry* geometry, KEY key, const DATA& data)
        {
            assert(phi::contains(_buckets, geometry));

            auto bucket = &_buckets[geometry];
            auto bucketIndex = getBucketIndex(bucket);
            auto indexInBucket = bucket->add(key);

            auto instanceIndex = bucketIndex + indexInBucket;
            _bufferData.insert(_bufferData.begin() + instanceIndex, data);

            uploadData(instanceIndex);
        }

        void update(geometry* geometry, KEY key, DATA data)
        {
            auto bucket = &_buckets[geometry];
            auto instanceIndex = getInstanceIndex(bucket, key);

            _bufferData[instanceIndex] = data;
            subData(_sizeOfDataType * instanceIndex, _sizeOfDataType, &data);
        }

        void removeInstance(geometry* geometry, KEY key)
        {
            auto bucket = &_buckets[geometry];
            auto instanceIndex = getInstanceIndex(bucket, key);

            bucket->remove(key);

            _bufferData.erase(_bufferData.begin() + instanceIndex);

            uploadData(instanceIndex);
        }

        void removeBucket(geometry* geometry)
        {
            auto bucket = &_buckets[geometry];
            assert(bucket->getSize() == 0);

            _buckets.erase(geometry);

            for (auto& pair : _buckets)
            {
                if (pair.second.getIndex() > bucket->getIndex())
                    pair.second.decrementIndex();
            }
        }
    };
}