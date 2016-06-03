#pragma once
#include <phi.h>
#include <core\rectangle.h>
#include <core\atlas.h>

#include "texture.h"
#include "textureAddress.h"
#include "textureContainerLayout.h"

namespace phi
{
    class textureContainer
    {
    private:
        size_t _freeSpace;
        bool _created;
        vec2 _texelSize;

        unordered_map<float, bool> _pages;
        unordered_map<float, atlas*> _atlases;

    protected:
        size_t _maxPages;
        textureContainerLayout _layout;
        GLint _unit;
        GLuint _id;
        GLuint64 _handle;
        vector<const texture*> _textures;
        map<const texture*, phi::textureAddress> _texturesAddresses;

    private:
        void create();
        void loadTexture(const texture* const texture);
        void subData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data);

        float getEmptyPage();
        float getAvailablePage();

    protected:
        virtual void onCreate();
        virtual void onLoadTexture(const texture* const texture);
        virtual void onSubData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data);

    public:
        textureContainer(
            textureContainerLayout layout,
            size_t maxPages);

        virtual ~textureContainer();

        bool add(const texture* const texture, textureAddress& textureAddress);
        bool subImage(const image* image, textureAddress& textureAddress);

        GLint getUnit() const { return _unit; }
        GLuint64 getHandle() const { return _handle; }
        vec2 getTexelSize() const { return _texelSize; }
    };
}
