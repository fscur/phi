#include <precompiled.h>
#include "texture.h"

namespace phi
{
    inline texture::texture(
        uint w, 
        uint h, 
        const textureLayout& layout,
        const void* data, 
        bool generateMipmaps,
        bool isAtlasTexture,
        GLenum type) :
        w(w),
        h(h),
        layout(layout),
        data(data),
        generateMipmaps(generateMipmaps),
        isAtlasTexture(isAtlasTexture),
        type(type)
    {
    }
    
    inline texture::texture(
        const image* image,
        const textureLayout& layout,
        bool generateMipmaps,
        bool isAtlasTexture,
        GLenum type) :
        w(image->w),
        h(image->h),
        layout(layout),
        data(image->data),
        generateMipmaps(generateMipmaps),
        isAtlasTexture(isAtlasTexture),
        type(type)
    {
        if (textureLayout::translateDataFormat(image->dataFormat) != layout.dataFormat)
            throw argumentException("image.dataFormat and layout.dataFormat must match.");

        if (textureLayout::translateDataType(image->dataType) != layout.dataType)
            throw argumentException("image.dataType and layout.dataType must match.");
    }
}