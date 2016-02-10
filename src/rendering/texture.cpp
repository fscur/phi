#include "texture.h"

#include <core/globals.h>
#include <io/path.h>

#include <stdio.h>

namespace phi
{
    texture* texture::_defaultAlbedo = nullptr;
    texture* texture::_defaultNormal = nullptr;
    texture* texture::_defaultSpecular = nullptr;
    texture* texture::_defaultEmissive = nullptr;

    GLuint64 texture::_defaultAlbedoHandle = 0;
    GLuint64 texture::_defaultNormalHandle = 0;
    GLuint64 texture::_defaultSpecularHandle = 0;
    GLuint64 texture::_defaultEmissiveHandle = 0;

    texture* texture::defaultAlbedo()
    {
        if (_defaultAlbedo == nullptr)
        {
            _defaultAlbedo = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });
            _defaultAlbedoHandle = _defaultAlbedo->handle;
        }

        return _defaultAlbedo;
    }

    texture* texture::defaultNormal()
    {
        if (_defaultNormal == nullptr)
        {
            _defaultNormal = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });
            _defaultNormalHandle = _defaultNormal->handle;
        }

        return _defaultNormal;
    }

    texture* texture::defaultSpecular()
    {
        if (_defaultSpecular == nullptr)
        {
            _defaultSpecular = createDefault(new byte[4]{ 0xFF, 0x80, 0xFF, 0xFF });
            _defaultSpecularHandle = _defaultSpecular->handle;
        }

        return _defaultSpecular;
    }

    texture* texture::defaultEmissive()
    {
        if (_defaultEmissive == nullptr)
        {
            _defaultEmissive = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });
            _defaultEmissiveHandle = _defaultEmissive->handle;
        }

        return _defaultEmissive;
    }

    texture* texture::createDefault(byte* data)
    {
        return new texture(1, 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);
    }

    void texture::bind(GLuint level)
    {
        glActiveTexture(GL_TEXTURE0 + level);
        glBindTexture(type, id);
    }

    void texture::setParam(GLenum name, GLint value)
    {
        glTexParameteri(type, name, value);
    }

    void texture::loadOnGpu()
    {
        if (_isLoadedOnGpu)
            return;

        glGenTextures(1, &id);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, id);

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, dataType, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        handle = glGetTextureHandleARB(id);
        glMakeTextureHandleResidentARB(handle);

        _isLoadedOnGpu = true;
    }

    void texture::releaseFromGpu()
    {
        if (!_isLoadedOnGpu)
            return;

        glMakeTextureHandleNonResidentARB(handle);
        glDeleteTextures(1, &id);
        _isLoadedOnGpu = false;
    }
}