#include <phi/core/globals.h>
#include <phi/io/path.h>
#include <phi/rendering/texture.h>

#include <stdio.h>

namespace phi
{
    texture* texture::defaultDiffuse = nullptr;
    texture* texture::defaultNormal = nullptr;
    texture* texture::defaultSpecular = nullptr;
    texture* texture::defaultEmissive = nullptr;

    texture* texture::getDefaultDiffuse()
    {
        if (defaultDiffuse == nullptr)
            defaultDiffuse = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return defaultDiffuse;
    }

    texture* texture::getDefaultNormal()
    {
        if (defaultNormal == nullptr)
            defaultNormal = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return defaultNormal;
    }

    texture* texture::getDefaultSpecular()
    {
        if (defaultSpecular == nullptr)
            defaultSpecular = createDefault(new byte[4]{ 0xFF, 0x80, 0xFF, 0xFF });

        return defaultSpecular;
    }

    texture* texture::getDefaultEmissive()
    {
        if (defaultEmissive == nullptr)
            defaultEmissive = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return defaultEmissive;
    }

    texture* texture::createDefault(byte* data)
    {
        return new texture(1, 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);
    }

    texture::texture(uint w, uint h, GLint internalFormat) :
        texture::texture(w, h, internalFormat, GL_RGBA, GL_UNSIGNED_BYTE, nullptr)
    {
    }

    texture::texture(uint w, uint h, GLint internalFormat, GLenum dataFormat, GLenum dataType, byte* data) :
        _w(w), _h(h), _internalFormat(internalFormat), _dataFormat(dataFormat), _dataType(dataType), _data(data)
    {
    }

    texture::~texture()
    {
    }

    void texture::bind(GLuint level)
    {
        glActiveTexture(GL_TEXTURE0 + level);
        glBindTexture(_textureType, _id);
    }

    void texture::setParam(GLenum name, GLint value)
    {
        glTexParameteri(_textureType, name, value);		
    }

    void texture::loadOnGpu()
    {
        GLuint id;
        glGenTextures(1, &id);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, id);

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _w, _h, 0, _dataFormat, _dataType, _data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        _loadedOnGpu = true;
    }

    void texture::releaseFromGpu()
    {
        glDeleteTextures(1, &_id);
        _loadedOnGpu = false;
    }
}