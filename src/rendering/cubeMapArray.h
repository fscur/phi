#pragma once
#include <phi.h>
#include <core\rectangle.h>
#include <core\atlas.h>

#include "cubeMap.h"
#include "textureAddress.h"
#include "textureLayout.h"

namespace phi
{
    class cubeMapArray
    {
    private:
        bool _created;
        unordered_map<float, bool> _pages;

    protected:
        sizeui _size;
        size_t _maxPages;
        textureLayout _layout;
        GLint _unit;
        GLuint _id;
        GLuint64 _handle;
        vector<const cubeMap*> _cubeMaps;
        map<const cubeMap*, phi::textureAddress> _cubeMapsAddresses;

    private:
        void create();
        bool loadData(
            const cubeMap* cubeMap,
            textureAddress& textureAddress);

        void loadData(
            float page,
            const cubeMap* cubeMap);

        float getAvailablePage();

    protected:
        virtual void onCreate();

        virtual void onLoadData(
            float page,
            const cubeMap* cubeMap);

    public:
        cubeMapArray(
            sizeui size,
            textureLayout layout);

        virtual ~cubeMapArray();

        bool add(const cubeMap* const cubeMap, textureAddress& cubeMapAddress);
        void remove(const cubeMap* cubeMap);
        bool isEmpty();
        void release();

        GLuint getId() const { return _id; }
        textureLayout getLayout() const { return _layout; }
        GLint getUnit() const { return _unit; }
        GLuint64 getHandle() const { return _handle; }
        //vec2 getTexelSize() const { return _texelSize; }
        sizeui getSize() const { return _size; }
    };
}
