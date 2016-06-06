#include <precompiled.h>
#include "bindlessTextureContainer.h"
#include "glError.h"

namespace phi
{
    bindlessTextureContainer::bindlessTextureContainer(
        sizeui size,
        textureLayout layout) :
        textureContainer(size, layout)
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
            _size.w,
            _size.h,
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

        _handle = glGetTextureHandleARB(_id);
        glError::check();
        glMakeTextureHandleResidentARB(_handle);
        glError::check();
    }

    void bindlessTextureContainer::onLoadData(
        float page,
        const void* const data)
    {
        if (data != nullptr)
        {
            glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
            glError::check();

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
            glError::check();

            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            glError::check();
        }
    }

    void bindlessTextureContainer::onLoadSubData(
        const rectangle<GLint>& rect,
        float page,
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