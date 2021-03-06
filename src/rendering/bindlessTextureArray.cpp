#include <precompiled.h>
#include "bindlessTextureArray.h"

namespace phi
{
    bindlessTextureArray::bindlessTextureArray(
        sizeui size,
        textureLayout layout) :
        textureArray(size, layout)
    {
    }

    bindlessTextureArray::~bindlessTextureArray()
    {
        glMakeTextureHandleNonResidentARB(_handle);
    }

    void bindlessTextureArray::onCreate()
    {
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

        _handle = glGetTextureHandleARB(_id);
        glMakeTextureHandleResidentARB(_handle);
    }

    void bindlessTextureArray::onLoadData(
        float page,
        const void* const data)
    {
        if (data != nullptr)
        {
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

    void bindlessTextureArray::onLoadSubData(
        const rectangle<GLint>& rect,
        float page,
        const void* const data)
    {
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
}