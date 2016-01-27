#include <phi\rendering\textureArray.h>

namespace phi
{
    textureArray::textureArray(sizeui size, GLint textureUnit) :
        _size(size), _textureUnit(textureUnit), _isLoadedOnGpu(false)
    {
    }

    void textureArray::loadOnGpu()
    {
        if (_isLoadedOnGpu)
            return;

        auto biggestTextureSize = (float)glm::max(_size.w, _size.h);
        auto maxLevels = glm::floor(glm::log2(biggestTextureSize)) + 1.0f;
        auto mipmapLevels = glm::min((uint)maxLevels, MAX_MIPMAP_LEVELS_ALLOWED);

        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);

        glTextureStorage3D(_id,
            mipmapLevels,
            GL_RGBA8,
            _size.w,
            _size.h,
            _size.d);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glActiveTexture(GL_TEXTURE0 + _textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);

        _isLoadedOnGpu = true;
    }

    void textureArray::add(texture* tex)
    {
        if (hasTexture(tex))
            return;

        auto index = (int)_textures.size();
        _textures[tex] = index;

        glActiveTexture(GL_TEXTURE0 + _textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
            0,
            0, 0,
            index,
            tex->getWidth(), tex->getHeight(),
            1,
            tex->getDataFormat(),
            tex->getDataType(),
            tex->getData());
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        //BINDLESS:
        //if(index == 0)
        //{
        //    _handle = glGetTextureHandleARB(_id);
        //    glMakeTextureHandleResidentARB(_handle);
        //}

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

        glMakeTextureHandleNonResidentARB(_handle); //TODO: check if this shit releases the handle from gpu!!!
        glDeleteTextures(1, &_id);
        _isLoadedOnGpu = false;
    }
}