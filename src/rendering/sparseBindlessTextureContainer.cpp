#include <precompiled.h>
#include "sparseBindlessTextureContainer.h"

namespace phi
{
    sparseBindlessTextureContainer::sparseBindlessTextureContainer(
        sizeui size,
        textureLayout layout) :
        textureContainer(size, layout)
    {
    }

    sparseBindlessTextureContainer::~sparseBindlessTextureContainer()
    {
        glMakeTextureHandleNonResidentARB(_handle);
    }

    void sparseBindlessTextureContainer::onCreate()
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);

        glTextureParameteri(_id, GL_TEXTURE_SPARSE_ARB, GL_TRUE);

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

        for (GLint i = 0; i < indexCount; ++i)
        {
            glTextureParameteri(_id, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, i);
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &xSize);
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &ySize);
            glGetInternalformativ(GL_TEXTURE_2D_ARRAY, internalFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &zSize);

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
        }

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

    void sparseBindlessTextureContainer::onLoadData(
        float page,
        const void* const data)
    {
        GLsizei levelWidth = _size.w;
        GLsizei levelHeight = _size.h;

        for (auto mipLevel = 0; mipLevel < _layout.levels; ++mipLevel)
        {
            glTexturePageCommitmentEXT(
                _id,
                mipLevel,
                0,
                0,
                static_cast<GLint>(page),
                levelWidth,
                levelHeight,
                1,
                GL_TRUE);
            

            levelWidth = std::max(levelWidth / 2, 1);
            levelHeight = std::max(levelHeight / 2, 1);
        }

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

    void sparseBindlessTextureContainer::onLoadSubData(
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