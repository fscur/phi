#include <precompiled.h>
#include "textureContainer.h"
#include "glError.h"

namespace phi
{
    textureContainer::textureContainer(
        textureContainerLayout layout,
        size_t maxTextures,
        GLint unit) :
        _layout(layout),
        _maxTextures(maxTextures),
        _freeSpace(maxTextures),
        _unit(unit),
        _created(false),
        id(0),
        handle(0),
        textures()
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);
        glError::check();
    }

    textureContainer::~textureContainer()
    {
        glDeleteTextures(1, &id);
        glError::check();
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

    bool textureContainer::add(
        const texture* const texture,
        textureAddress& textureAddress)
    {
        if (_freeSpace == 0)
            return false;

        if (phi::contains(textures, texture))
        {
            textureAddress.containerId = texturesAddresses[texture].containerId;
            textureAddress.unit = texturesAddresses[texture].unit;
            textureAddress.page = texturesAddresses[texture].page;
            return true;
        }

        auto page = textures.size();
        textureAddress.containerId = id;
        textureAddress.unit = _unit;
        textureAddress.page = static_cast<float>(page);

        textures.push_back(texture);
        texturesAddresses[texture] = textureAddress;
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

    void textureContainer::onCreate()
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glError::check();

        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glError::check();

        glTextureStorage3D(id,
            _layout.levels,
            _layout.internalFormat,
            _layout.w,
            _layout.h,
            static_cast<GLsizei>(_maxTextures));
        glError::check();

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, _layout.wrapMode);
        glError::check();
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, _layout.wrapMode);
        glError::check();
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _layout.minFilter);
        glError::check();
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _layout.magFilter);
        glError::check();
    }

    void textureContainer::onLoadTexture(const texture* const texture)
    {
        auto textureAddress = texturesAddresses[texture];

        if (texture->data != nullptr)
        {
            glActiveTexture(GL_TEXTURE0 + _unit);
            glError::check();

            glBindTexture(GL_TEXTURE_2D_ARRAY, id);
            glError::check();

            glTextureSubImage3D(
                id,
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

        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glError::check();

        glTextureSubImage3D(
            id,
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
}