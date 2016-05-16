#pragma once
#include <phi.h>
#include <core\image.h>

namespace phi
{
    struct texture
    {
    public:
        GLuint id;
        GLuint64 handle;
        GLuint w;
        GLuint h;
        GLenum dataFormat;
        GLenum dataType;
        byte* data;
        GLenum type;
        GLenum internalFormat;
        GLint wrapMode;
        GLint minFilter;
        GLint magFilter;
        bool generateMipmaps;

    public:
        texture(
            image* image,
            GLenum type = GL_TEXTURE_2D,
            GLenum internalFormat = GL_RGBA8,
            GLint wrapMode = GL_REPEAT,
            GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
            GLint magFilter = GL_LINEAR,
            bool generateMipmaps = true) :
            type(type),
            internalFormat(internalFormat),
            wrapMode(wrapMode),
            minFilter(minFilter),
            magFilter(magFilter),
            generateMipmaps(generateMipmaps)
        {
			w = image->w;
			h = image->h;

			switch (image->dataFormat)
			{
			case imageDataFormat::bgra:
				dataFormat = GL_BGRA;
				break;
			case imageDataFormat::bgr:
				dataFormat = GL_BGR;
				break;
			case imageDataFormat::rgba:
				dataFormat = GL_RGBA;
				break;
			case imageDataFormat::rgb:
				dataFormat = GL_RGB;
				break;
			default:
				break;
			}

			switch (image->dataType)
			{
			case imageDataType::ubyte_dataType:
				dataType = GL_UNSIGNED_BYTE;
				break;
			case imageDataType::float_dataType:
				dataType = GL_FLOAT;
				break;
			default:
				break;
			}

			data = image->data;
        }

		texture(
			uint w,
			uint h,
			GLenum dataFormat = GL_BGRA,
			GLenum dataType = GL_UNSIGNED_BYTE,
			byte* data = nullptr,
			GLenum type = GL_TEXTURE_2D,
			GLenum internalFormat = GL_RGBA8,
			GLint wrapMode = GL_REPEAT,
			GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
			GLint magFilter = GL_LINEAR,
			bool generateMipmaps = true) :
			w(w),
			h(h),
			dataFormat(dataFormat),
			dataType(dataType),
			data(data),
			type(type),
			internalFormat(internalFormat),
			wrapMode(wrapMode),
			minFilter(minFilter),
			magFilter(magFilter),
			generateMipmaps(generateMipmaps)
		{
		}

        ~texture() 
        {
        }
    };
}