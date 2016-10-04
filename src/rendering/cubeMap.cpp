#include <precompiled.h>
#include "cubeMap.h"

namespace phi
{
    inline cubeMap::cubeMap(
        uint w,
        uint h,
        const textureLayout& layout,
        vector<const void*> data) :
        w(w),
        h(h),
        layout(layout),
        data(data)
    {
    }

    inline cubeMap::cubeMap(
        vector<const image*> images,
        const textureLayout& layout) :
        layout(layout)
    {
        w = images[0]->w;
        h = images[0]->h;
        
        for (auto& image : images)
        {
            if (textureLayout::translateDataFormat(image->dataFormat) != layout.dataFormat)
                throw argumentException("image.dataFormat and layout.dataFormat must match.");

            if (textureLayout::translateDataType(image->dataType) != layout.dataType)
                throw argumentException("image.dataType and layout.dataType must match.");

            data.push_back(image->data);
        }
    }
}