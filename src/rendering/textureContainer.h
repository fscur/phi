#pragma once
#include <phi.h>
#include <core\rectangle.h>
#include <core\atlas.h>

#include "texture.h"
#include "textureAddress.h"
#include "textureLayout.h"

namespace phi
{
    class textureContainer
    {
    private:
        bool _created;
        vec2 _texelSize;

        unordered_map<float, bool> _pages;
        unordered_map<float, atlas*> _atlases;

    protected:
        sizeui _size;
        size_t _maxPages;
        textureLayout _layout;
        GLint _unit;
        GLuint _id;
        GLuint64 _handle;
        vector<const texture*> _textures;
        map<const texture*, phi::textureAddress> _texturesAddresses;

    private:
        void create();
        bool loadData(
            const texture* const texture, 
            textureAddress& textureAddress);

        void loadData(
            float page, 
            const void* const data);

        bool loadSubData(
            const texture* texture, 
            textureAddress& textureAddress);

        void loadSubData(
            const rectangle<GLint>& rect, 
            float page, 
            const void* const data);

        float getEmptyPage();
        float getAvailablePage();

    protected:
        virtual void onCreate();

        virtual void onLoadData(
            float page,
            const void* const data);

        virtual void onLoadSubData(
            const rectangle<GLint>& rect, 
            float page, 
            const void* const data);

    public:
        textureContainer(
            sizeui size,
            textureLayout layout);

        virtual ~textureContainer();

        bool add(const texture* const texture, textureAddress& textureAddress);

        GLint getUnit() const { return _unit; }
        GLuint64 getHandle() const { return _handle; }
        vec2 getTexelSize() const { return _texelSize; }
        sizeui getSize() const { return _size; }
    };
}
