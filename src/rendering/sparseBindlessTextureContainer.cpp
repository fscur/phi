#include <precompiled.h>
#include "sparseBindlessTextureContainer.h"
#include "glError.h"

namespace phi
{
    sparseBindlessTextureContainer::sparseBindlessTextureContainer(
        textureContainerLayout layout,
        size_t maxTextures) :
        textureContainer(layout, maxTextures)
    {
    }

    sparseBindlessTextureContainer::~sparseBindlessTextureContainer()
    {
        glMakeTextureHandleNonResidentARB(_handle);
        glError::check();
    }

    void sparseBindlessTextureContainer::onCreate()
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);
        glError::check();

        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        glError::check();

        glTextureParameteri(_id, GL_TEXTURE_SPARSE_ARB, GL_TRUE);
        glError::check();

        // TODO: This could be done once per internal format. For now, just do it every time.
        GLint indexCount = 0,
            xSize = 0,
            ySize = 0,
            zSize = 0;

        GLint bestIndex = -1,
            bestXSize = 0,
            bestYSize = 0;

        auto internalFormat = _layout.internalFormat;

        glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &indexCount);
        glError::check();

        for (GLint i = 0; i < indexCount; ++i)
        {
            glTextureParameteri(_id, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, i);
            glError::check();
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &xSize);
            glError::check();
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &ySize);
            glError::check();
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &zSize);
            glError::check();

            if (zSize == 1)
            {
                if (xSize >= bestXSize && ySize >= bestYSize) {
                    bestIndex = i;
                    bestXSize = xSize;
                    bestYSize = ySize;
                }
            }
        }

        if (bestIndex != -1)
        {
            glTextureParameteri(_id, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, bestIndex);
            glError::check();
        }

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

        _handle = glGetTextureHandleARB(_id);
        glError::check();
        glMakeTextureHandleResidentARB(_handle);
        glError::check();
    }

    void sparseBindlessTextureContainer::onLoadTexture(const texture* const texture)
    {
        auto textureAddress = _texturesAddresses[texture];

        GLsizei levelWidth = _layout.w;
        GLsizei levelHeight = _layout.h;

        for (auto mipLevel = 0; mipLevel < _layout.levels; ++mipLevel)
        {
            glTexturePageCommitmentEXT(
                _id,
                mipLevel,
                0,
                0,
                static_cast<GLint>(textureAddress.page),
                levelWidth,
                levelHeight,
                1,
                GL_TRUE);
            glError::check();

            levelWidth = std::max(levelWidth / 2, 1);
            levelHeight = std::max(levelHeight / 2, 1);
        }

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

    void sparseBindlessTextureContainer::onSubData(
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