#include <precompiled.h>
#include "image.h"

namespace phi
{
    image* image::defaultAlbedoImage = image::createDefaultImage(vec4(1.0f));
    image* image::defaultNormalImage = image::createDefaultImage(vec4(0.5f, 0.5f, 1.0f, 1.0f));
    image* image::defaultSpecularImage = image::createDefaultImage(vec4(1.0f));
    image* image::defaultEmissiveImage = image::createDefaultImage(vec4(0.0f));

    image* image::createDefaultImage(const vec4 & color)
    {
        //default texture size is 128 x 128 because of sparse textures

        auto w = 128u;
        auto h = 128u;

        auto data = new byte[w * h * 4];

        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h * 4; y += 4)
            {
                auto index = (y + h * x * 4);

                data[index + 0] = static_cast<byte>(color.r * 255);
                data[index + 1] = static_cast<byte>(color.g * 255);
                data[index + 2] = static_cast<byte>(color.b * 255);
                data[index + 3] = static_cast<byte>(color.a * 255);
            }
        }

        return new image(guid::newGuid(), w, h, imageDataFormat::rgba, imageDataType::ubyte_dataType, data);
    }

    void image::release()
    {
        safeDelete(defaultAlbedoImage);
        safeDelete(defaultNormalImage);
        safeDelete(defaultSpecularImage);
        safeDelete(defaultEmissiveImage);
    }
}