#pragma once
#include <phi.h>
#include <core\image.h>
#include <core\argumentException.h>
#include "renderingApi.h"
#include "textureLayout.h"

namespace phi
{
    struct texture
    {
    public:
        GLuint w;
        GLuint h;
        textureLayout layout;
        const void* data;
        bool generateMipmaps;
        bool isAtlasTexture;
        GLenum type;

    public:
        RENDERING_API texture(
            uint w,
            uint h,
            const textureLayout& layout, 
            const void* data = nullptr,
            bool generateMipmaps = false,
            bool isAtlasTexture = false,
            GLenum type = GL_TEXTURE_2D);

        RENDERING_API texture(
                const image* image,
                const textureLayout& layout,
                bool generateMipmaps = false,
                bool isAtlasTexture = false,
                GLenum type = GL_TEXTURE_2D);
    };
}