#include "textureContainer.h"

namespace phi
{
    bool textureContainer::add(texture* texture, textureAddress& textureAddress)
    {
        if (_freeSpace == 0)
            return false;

        if (phi::contains(textures, texture))
        {
            textureAddress.unit = texturesAddresses[texture].unit;
            textureAddress.page = texturesAddresses[texture].page;
            return true;
        }

        auto page = textures.size();

        textureAddress.unit = _unit;
        textureAddress.page = static_cast<float>(page);

        textures.push_back(texture);
        texturesAddresses[texture] = textureAddress;
        --_freeSpace;

        return true;
    }

    void textureContainer::load()
    {
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);

        if (!_bindless)
            glActiveTexture(GL_TEXTURE0 + _unit);

        glBindTexture(GL_TEXTURE_2D_ARRAY, id);

        if (_sparse)
        {
            glTextureParameteri(id, GL_TEXTURE_SPARSE_ARB, GL_TRUE);

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
                glTextureParameteri(id, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, i);
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
                glTextureParameteri(id, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, bestIndex);
        }

        glTextureStorage3D(id,
            _layout.levels,
            _layout.internalFormat,
            _layout.w,
            _layout.h,
            static_cast<GLsizei>(textures.size()));

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        auto texturesCount = static_cast<uint>(textures.size());

        for (auto arrayLayer = 0u; arrayLayer < texturesCount; ++arrayLayer)
        {
            auto texture = textures[arrayLayer];

            if (_sparse)
            {
                GLsizei levelWidth = _layout.w;
                GLsizei levelHeight = _layout.h;

                for (auto mipLevel = 0; mipLevel < _layout.levels; ++mipLevel)
                {
                    glTexturePageCommitmentEXT(
                        id,
                        mipLevel,
                        0,
                        0,
                        arrayLayer,
                        levelWidth,
                        levelHeight,
                        1,
                        GL_TRUE);

                    levelWidth = std::max(levelWidth / 2, 1);
                    levelHeight = std::max(levelHeight / 2, 1);
                }
            }

            glTextureSubImage3D(
                id,
                0,
                0,
                0,
                arrayLayer,
                texture->w,
                texture->h,
                1,
                texture->dataFormat,
                texture->dataType,
                texture->data);
        }

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        if (_bindless)
        {
            handle = glGetTextureHandleARB(id);
            glMakeTextureHandleResidentARB(handle);
        }
    }

    void textureContainer::unload()
    {
        if (_bindless)
            glMakeTextureHandleNonResidentARB(handle); //TODO: check if this shit releases the handle from gpu!!!

        glDeleteTextures(1, &id);
    }
}