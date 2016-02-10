#include "textureArray.h"

namespace phi
{
    void textureArray::loadOnGpu()
    {
        if (_isLoadedOnGpu)
            return;

        auto biggestTextureSize = (float)glm::max(w, h);
        auto maxLevels = glm::floor(glm::log2(biggestTextureSize)) + 1.0f;
        auto mipmapLevels = glm::min((uint)maxLevels, MAX_MIPMAP_LEVELS_ALLOWED);

        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);

        glTextureStorage3D(id,
            mipmapLevels,
            GL_RGBA8,
            w,
            h,
            texCount);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);

        _isLoadedOnGpu = true;
    }

    void textureArray::add(texture* tex)
    {
        if (hasTexture(tex))
            return;

        auto index = (int)_textures.size();
        _textures[tex] = index;

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
            0,
            0, 0,
            index,
            tex->w, tex->h,
            1,
            tex->dataFormat,
            tex->dataType,
            tex->data);

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        if(index == 0)
        {
            handle = glGetTextureHandleARB(id);
            glMakeTextureHandleResidentARB(handle);
        }

        return;
    }

    void textureArray::remove(texture* tex)
    {
        _textures.erase(tex);
    }

    bool textureArray::hasTexture(texture* tex)
    {
        return _textures.find(tex) != _textures.end();
    }

    int textureArray::getTextureIndex(texture* tex)
    {
        return _textures[tex];
    }

    void textureArray::releaseFromGpu()
    {
        if (!_isLoadedOnGpu)
            return;

        glMakeTextureHandleNonResidentARB(handle); //TODO: check if this shit releases the handle from gpu!!!
        glDeleteTextures(1, &id);
        _isLoadedOnGpu = false;
    }
}