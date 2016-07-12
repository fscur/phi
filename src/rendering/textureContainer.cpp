#include <precompiled.h>
#include "textureContainer.h"
#include "textureUnits.h"

namespace phi
{
    textureContainer::textureContainer(
        sizeui size,
        textureLayout layout) :
        _created(false),
        _texelSize(vec2(0.0f)),
        _pages(unordered_map<float, bool>()),
        _atlases(unordered_map<float, atlas*>()),
        _size(size),
        _maxPages(size.d),
        _layout(layout),
        _unit(-1),
        _id(0),
        _handle(0),
        _textures(vector<const texture*>()),
        _texturesAddresses(map<const texture*, phi::textureAddress>())
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);

        _unit = textureUnits::get();

        for (auto i = 0; i < _maxPages; ++i)
            _pages[static_cast<float>(i)] = false;

        _texelSize = vec2(1.0f / (float)size.w, 1.0f / (float)size.h);
    }

    textureContainer::~textureContainer()
    {
        glDeleteTextures(1, &_id);

        textureUnits::clear(_unit);

        for (auto& pair : _atlases)
            safeDelete(pair.second);
    }

    void textureContainer::create()
    {
        onCreate();

        _created = true;
    }

    bool textureContainer::loadData(
        const texture * const texture, 
        textureAddress & textureAddress)
    {
        float page = getEmptyPage();

        if (page < 0.0f)
            return false;

        textureAddress.containerId = _id;
        textureAddress.unit = _unit;
        textureAddress.page = page;
        textureAddress.rect = rectangle<uint>(0, 0, texture->w, texture->h);

        _textures.push_back(texture);
        _texturesAddresses[texture] = textureAddress;

        loadData(page, texture->data);

        return true;
    }

    void textureContainer::loadData(
        float page,
        const void* const data)
    {
        onLoadData(page, data);
    }

    bool textureContainer::loadSubData(
        const texture* texture, 
        textureAddress& textureAddress)
    {
        auto page = getAvailablePage();

        if (page < 0.0f)
            return false;

        auto rect = rectangle<uint>(0, 0, texture->w, texture->h);
        auto atlasItem = new phi::atlasItem(rect, texture);

        phi::atlasNode* atlasNode = nullptr;

        while (atlasNode == nullptr && page < _maxPages)
        {
            auto atlas = _atlases[page];
            if (!atlas)
            {
                atlas = new phi::atlas(_size);
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
            loadSubData(subDataRect, page, texture->data);

            textureAddress = phi::textureAddress();
            textureAddress.containerId = _id;
            textureAddress.unit = _unit;
            textureAddress.page = page;
            textureAddress.rect = atlasNode->rect;

            return true;
        }

        return false;
    }

    void textureContainer::loadSubData(
        const rectangle<GLint>& rect, 
        float page, 
        const void* const data)
    {
        onLoadSubData(rect, page, data);
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

    void textureContainer::onCreate()
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        
        glTextureStorage3D(_id,
            _layout.levels,
            _layout.internalFormat,
            _size.w,
            _size.h,
            static_cast<GLsizei>(_maxPages));
        

        glTextureParameteri(_id, GL_TEXTURE_WRAP_S, _layout.wrapMode);
        glTextureParameteri(_id, GL_TEXTURE_WRAP_T, _layout.wrapMode);
        glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, _layout.minFilter);
        glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, _layout.magFilter);
    }

    void textureContainer::onLoadData(
        float page,
        const void* const data)
    {
        if (data != nullptr)
        {
            glActiveTexture(GL_TEXTURE0 + _unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
            glTextureSubImage3D(
                _id,
                0,
                0,
                0,
                static_cast<GLint>(page),
                _size.w,
                _size.h,
                1,
                _layout.dataFormat,
                _layout.dataType,
                data);

            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        }
    }

    void textureContainer::onLoadSubData(
        const rectangle<GLint>& rect,
        float page,
        const void* const data)
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
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

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    bool textureContainer::add(
        const texture* const texture,
        textureAddress& textureAddress)
    {
        if (phi::contains(_textures, texture))
        {
            textureAddress = _texturesAddresses[texture];
            return true;
        }

        if (!_created)
            create();

        if (!texture->isAtlasTexture)
            return loadData(texture, textureAddress);
        else
            return loadSubData(texture, textureAddress);
    }

    void textureContainer::remove(const texture * texture)
    {
        auto address = _texturesAddresses[texture];
        
        phi::removeIfContains(_textures, texture);
        _texturesAddresses.erase(texture);

        _pages[address.page] = false;

        safeDelete(texture);

        //TODO:remove sparse page
    }

    bool textureContainer::isEmpty()
    {
        return _textures.size() == 0;
    }

    void textureContainer::release()
    {
        glDeleteTextures(1, &_id);
        textureUnits::clear(_unit);
    }
}