#pragma once
#include <phi.h>

namespace phi
{
    namespace imageDataFormat
    {
        enum imageDataFormat
        {
            rgb,
            rgba,
            rgba8,
            bgr,
            bgra
        };
    }

    namespace imageDataType
    {
        enum imageDataType
        {
            ubyte_dataType,
            float_dataType
        };
    }

    struct image
    {
        uint w;
        uint h;
        imageDataFormat::imageDataFormat dataFormat;
        imageDataType::imageDataType dataType;
        void* data;

        image(
            uint w = 0u,
            uint h = 0u,
            imageDataFormat::imageDataFormat dataFormat = imageDataFormat::rgba8,
            imageDataType::imageDataType dataType = imageDataType::ubyte_dataType,
            void* data = nullptr) :
            w(w),
            h(h),
            dataFormat(dataFormat),
            dataType(dataType),
            data(data)
        {
        }

        ~image()
        {
            safeDeleteArray(data);
        }
    };
}