#include <precompiled.h>
#include "textureContainer.h"
#include "glError.h"
#include "textureUnits.h"

namespace phi
{
    textureContainer::textureContainer(
        textureContainerLayout layout,
        size_t maxPages) :
        _freeSpace(maxPages),
        _created(false),
        _maxPages(maxPages),
        _layout(layout),
        _unit(-1),
        _id(0),
        _handle(0),
        _textures(vector<const texture*>()),
        _texturesAddresses(map<const texture*, phi::textureAddress>())
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);
        glError::check();

        _unit = textureUnits::get();

        for (auto i = 0; i < maxPages; ++i)
            _pages[static_cast<float>(i)] = false;

        _texelSize = vec2(1.0f / (float)layout.w, 1.0f / (float)layout.h);
    }

    textureContainer::~textureContainer()
    {
        glDeleteTextures(1, &_id);
        glError::check();

        textureUnits::clear(_unit);
    }

    void textureContainer::create()
    {
        onCreate();

        _created = true;
    }

    void textureContainer::loadTexture(const texture* const texture)
    {
        if (!_created)
            create();

        onLoadTexture(texture);
    }

    void textureContainer::onCreate()
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glError::check();

        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        glError::check();

        glTextureStorage3D(_id,
            _layout.levels,
            _layout.internalFormat,
            _layout.w,
            _layout.h,
            static_cast<GLsizei>(_maxPages));
        glError::check();

        glTextureParameteri(_id, GL_TEXTURE_WRAP_S, _layout.wrapMode);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_WRAP_T, _layout.wrapMode);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, _layout.minFilter);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, _layout.magFilter);
        glError::check();
    }

    void textureContainer::onLoadTexture(const texture* const texture)
    {
        auto textureAddress = _texturesAddresses[texture];

        if (texture->data != nullptr)
        {
            glActiveTexture(GL_TEXTURE0 + _unit);
            glError::check();

            glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
            glError::check();

            glTextureSubImage3D(
                _id,
                0,
                0,
                0,
                static_cast<GLint>(textureAddress.page),
                texture->w,
                texture->h,
                1,
                texture->dataFormat,
                texture->dataType,
                texture->data);
            glError::check();

            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            glError::check();
        }
    }

    void textureContainer::onSubData(
        const float& page,
        const rectangle<GLint>& rect,
        const void* const data)
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glError::check();

        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        glError::check();

        glTextureSubImage3D(
            _id,
            0,
            rect.x,
            rect.y,
            static_cast<GLint>(page),
            rect.w,
            rect.h,
            1,
            _layout.dataFormat,
            _layout.dataType,
            data);

        glError::check();

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glError::check();
    }

    bool textureContainer::add(
        const texture* const texture,
        textureAddress& textureAddress)
    {
        if (_freeSpace == 0)
            return false;

        if (phi::contains(_textures, texture))
        {
            textureAddress = _texturesAddresses[texture];
            return true;
        }

        float page = getEmptyPage();

        if (page < 0.0f)
            return false;
        
        textureAddress.containerId = _id;
        textureAddress.unit = _unit;
        textureAddress.page = page;
        textureAddress.rect = rectangle<uint>(0, 0, texture->w, texture->h);

        _textures.push_back(texture);
        _texturesAddresses[texture] = textureAddress;
        --_freeSpace;

        loadTexture(texture);

        return true;
    }

    void textureContainer::subData(
        const float& page,
        const rectangle<GLint>& rect,
        const void* const data)
    {
        onSubData(page, rect, data);
    }

    bool textureContainer::subImage(
        const image* image,
        textureAddress& textureAddress)
    {
        if (!_created)
            create();

        auto page = getAvailablePage();

        if (page < 0.0f)
            return false;
 
        auto rect = rectangle<uint>(0, 0, image->w, image->h);
        auto atlasItem = new phi::atlasItem(rect, image);

        phi::atlasNode* atlasNode = nullptr;

        while (atlasNode == nullptr && page < _maxPages)
        {
            auto atlas = _atlases[page];
            if (!atlas)
            {
 
                atlas = new phi::atlas(sizeui(_layout.w, _layout.h));
                _atlases[page] = atlas;
            }

            atlasNode = atlas->insert(atlasItem);

            if (atlas->isFull())
                _pages[page++] = true;
        };

        if (atlasNode)
        {
            auto x = atlasNode->rect.x;
            auto y = atlasNode->rect.y;
            auto w = atlasNode->rect.w;
            auto h = atlasNode->rect.h;

            auto subDataRect = rectangle<GLint>(x, y, w, h);
            subData(page, subDataRect, image->data);

            textureAddress = phi::textureAddress();
            textureAddress.containerId = _id;
            textureAddress.unit = _unit;
            textureAddress.page = page;
            textureAddress.rect = atlasNode->rect;

            return true;
        }

        return false;
    }

    float textureContainer::getEmptyPage()
    {
        for (auto i = 0; i < _maxPages; ++i)
        {
            auto key = static_cast<float>(i);
            bool pageNotUsed = !_pages[key];
            bool isNotAtlas = !contains(_atlases, key);
            if (pageNotUsed && isNotAtlas)
            {
                _pages[key] = true;
                return key;
            }
        }

        return -1.0f;
    }

    float textureContainer::getAvailablePage()
    {
        for (auto i = 0; i < _maxPages; ++i)
        {
            auto key = static_cast<float>(i);
            bool pageNotUsed = !_pages[key];
            bool isAtlas = contains(_atlases, key);
            bool atlasNotFull = isAtlas &&!_atlases[key]->isFull();

            if (pageNotUsed || atlasNotFull)
                return key;
        }

        return -1.0f;
    }
}