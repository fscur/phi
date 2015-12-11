#include <phi/core/globals.h>
#include <phi/io/path.h>
#include <phi/rendering/texture.h>

#include <stdio.h>
#include <SDL/SDL_image.h>

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
        auto t = texture::create(1, 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, data);
        safeDeleteArray(data);
        return t;
    }

    texture::texture(GLuint id, uint w, uint h)
    {
        _id = id;
        _w = w;
        _h = h;
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

    void texture::release()
    {
        glDeleteTextures(1, &_id);
    }

    texture* texture::fromFile(std::string fileName)
    {
        SDL_Surface* surface = IMG_Load(fileName.c_str());

        SDL_InvertSurface(surface);

        GLuint id, width, height;
        glGenTextures(1, &id);

        width = surface->w;
        height = surface->h;

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, id);

        GLuint format = 0;
        switch (surface->format->BitsPerPixel)
        {
        case 24:
            if (surface->format->Rmask == 255)
                format = GL_RGB;
            else 
                format = GL_BGR;
            break;
        case 32:
            if (surface->format->Rmask == 255)
                format = GL_RGBA;
            else
                format = GL_BGRA;
            break;
        default:
            break;
        }

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
        glGenerateMipmap(GL_TEXTURE_2D);

        SDL_FreeSurface(surface);

        auto t =  new texture(id, width, height);
        t->_textureType = GL_TEXTURE_2D;

        return t;
    }

    texture* texture::create(uint w, uint h, GLint internalFormat, GLint format, GLint type, GLuint level, GLvoid* data)
    {
        GLuint id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, w, h, 0, format, type, data);
        auto t = new texture(id, w, h);

        t->_textureType = GL_TEXTURE_2D;

        return t;
    }

    texture* texture::createCubeMap(uint w, uint h, GLint internalFormat, GLint format, GLint type, GLuint level, const std::vector<GLvoid*> data)
    {
        GLuint id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        bool hasData = data.size() > 0;

        for (unsigned int i = 0 ; i < 6 ; i++) 
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, internalFormat, w, h, 0, format, type, hasData ? data[i] : 0);

        auto t = new texture(id, w, h);
        t->_textureType = GL_TEXTURE_CUBE_MAP;
        return t;
    }
}