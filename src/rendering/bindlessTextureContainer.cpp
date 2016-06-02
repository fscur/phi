#include <precompiled.h>
#include "bindlessTextureContainer.h"
#include "glError.h"

namespace phi
{
    bindlessTextureContainer::bindlessTextureContainer(
        textureContainerLayout layout,
        size_t maxTextures) :
        textureContainer(layout, maxTextures)
    {
    }

    bindlessTextureContainer::~bindlessTextureContainer()
    {
        glMakeTextureHandleNonResidentARB(_handle);
        glError::check();
    }

    void bindlessTextureContainer::onCreate()
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        glError::check();

        glTextureStorage3D(_id,
            _layout.levels,
            _layout.internalFormat,
            _layout.w,
            _layout.h,
            static_cast<GLsizei>(_maxTextures));
        glError::check();

        glTextureParameteri(_id, GL_TEXTURE_WRAP_S, _layout.wrapMode);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_WRAP_T, _layout.wrapMode);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, _layout.minFilter);
        glError::check();
        glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, _layout.magFilter);
        glError::check();

        _handle = glGetTextureHandleARB(_id);
        glError::check();
        glMakeTextureHandleResidentARB(_handle);
        glError::check();
    }

    void bindlessTextureContainer::onLoadTexture(const texture* const texture)
    {
        auto textureAddress = _texturesAddresses[texture];

        if (texture->data != nullptr)
        {
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

    void bindlessTextureContainer::onSubData(
        const float& page,
        const rectangle<GLint>& rect,
        const void* const data)
    {
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
}