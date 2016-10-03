#pragma once
#include <phi.h>
#include "coreApi.h"

#include "entity.h"
#include "guid.h"

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

    struct image :
        public entity
    {
    public:
        CORE_API static image* defaultAlbedoImage;
        CORE_API static image* defaultNormalImage;
        CORE_API static image* defaultSpecularImage;
        CORE_API static image* defaultEmissiveImage;

    public:
        uint w;
        uint h;
        imageDataFormat::imageDataFormat dataFormat;
        imageDataType::imageDataType dataType;
        byte* data;

        image(
            guid guid,
            uint w = 0u,
            uint h = 0u,
            imageDataFormat::imageDataFormat dataFormat = imageDataFormat::rgba8,
            imageDataType::imageDataType dataType = imageDataType::ubyte_dataType,
            byte* data = nullptr) :
            entity::entity(guid),
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

    private:
        static image* createDefaultImage(const vec4 & color);

    public:
        CORE_API static void release();
    };
}