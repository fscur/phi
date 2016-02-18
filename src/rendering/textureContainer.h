#pragma once

#include "texture.h"
#include <map>

namespace phi
{
    struct textureAddress
    {
        GLint unit;
        GLfloat page;
    };

    struct textureContainerLayout
    {
        GLsizei w;
        GLsizei h;
        GLsizei levels;
        GLenum internalFormat;

        textureContainerLayout(
            GLsizei w = 0,
            GLsizei h = 0,
            GLsizei levels = 0,
            GLenum internalFormat = GL_RGBA8) :
            w(w),
            h(h),
            levels(levels),
            internalFormat(internalFormat)
        {
        }
    };

    class textureContainer
    {
    private:
        textureContainerLayout _layout;
        uint _maxTextures;
        uint _freeSpace;
        GLint _unit;
        bool _bindless;
        bool _sparse;

    public:
        GLuint id;
        GLuint64 handle;
        std::map<texture*, textureAddress> texturesAddresses;
        std::vector<texture*> textures;

        textureContainer(
            textureContainerLayout layout,
            uint maxTextures,
            GLint unit,
            bool bindless = false,
            bool sparse = false) :
            _layout(layout),
            _maxTextures(maxTextures),
            _freeSpace(maxTextures),
            _unit(unit),
            _bindless(bindless),
            _sparse(sparse),
            id(0),
            handle(0),
            textures()
        {
        }

        bool add(texture* texture, textureAddress& textureAddress);
        void load();
        void unload();
    };
}

