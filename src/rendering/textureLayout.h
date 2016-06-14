#pragma once
#include <phi.h>
#include <functional>
#include <core\image.h>
#include <core\argumentException.h>

namespace phi
{
    struct textureLayout
    {
        GLenum dataFormat;
        GLenum dataType;
        GLsizei levels;
        GLenum internalFormat;
        GLint wrapMode;
        GLint minFilter;
        GLint magFilter;

        textureLayout(
            GLenum dataFormat = GL_BGRA,
            GLenum dataType = GL_UNSIGNED_BYTE,
            GLsizei levels = 1,
            GLenum internalFormat = GL_RGBA8,
            GLint wrapMode = GL_REPEAT,
            GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
            GLint magFilter = GL_LINEAR) :
            dataFormat(dataFormat),
            dataType(dataType),
            levels(levels),
            internalFormat(internalFormat),
            wrapMode(wrapMode),
            minFilter(minFilter),
            magFilter(magFilter)
        {
        }

        static GLenum translateDataFormat(const imageDataFormat::imageDataFormat& dataFormat)
        {
            switch (dataFormat)
            {
            case imageDataFormat::imageDataFormat::bgra:
                return GL_BGRA;
            case imageDataFormat::imageDataFormat::bgr:
                return GL_BGR;
            case imageDataFormat::imageDataFormat::rgba:
                return GL_RGBA;
            case imageDataFormat::imageDataFormat::rgb:
                return GL_RGB;
            default:
                throw argumentException("invalid dataFormat");
            }
        }

        static GLenum translateDataType(const imageDataType::imageDataType& dataType)
        {
            switch (dataType)
            {
            case imageDataType::imageDataType::float_dataType:
                return GL_FLOAT;
            case imageDataType::imageDataType::ubyte_dataType:
                return GL_UNSIGNED_BYTE;
            default:
                throw argumentException("invalid dataType");
            }
        }

        bool operator==(const textureLayout &other) const
        {
            return (dataFormat == other.dataFormat
                && dataType == other.dataType
                && levels == other.levels
                && internalFormat == other.internalFormat
                && wrapMode == other.wrapMode
                && minFilter == other.minFilter
                && magFilter == other.magFilter);
        }
    };
}

namespace std 
{
    //http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
    template <>
    struct hash<phi::textureLayout>
    {
        std::size_t operator()(const phi::textureLayout& layout) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            std::size_t seed = 0;

            std::hash_combine(seed, hash_value(layout.dataFormat));
            std::hash_combine(seed, hash_value(layout.dataType));
            std::hash_combine(seed, hash_value(layout.levels));
            std::hash_combine(seed, hash_value(layout.internalFormat));
            std::hash_combine(seed, hash_value(layout.wrapMode));
            std::hash_combine(seed, hash_value(layout.minFilter));
            std::hash_combine(seed, hash_value(layout.magFilter));

            return seed;
        }
    };
}