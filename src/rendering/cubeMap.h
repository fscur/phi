#pragma once
#include <phi.h>
#include <core\image.h>
#include <core\argumentException.h>

#include "renderingApi.h"
#include "textureLayout.h"

namespace phi
{
    struct cubeMap
    {
    public:
        GLuint w;
        GLuint h;
        textureLayout layout;
        vector<const void*> data;

    public:
        RENDERING_API cubeMap(
            uint w,
            uint h,
            const textureLayout& layout,
            vector<const void*> data);

        RENDERING_API cubeMap(
            vector<const image*> images,
            const textureLayout& layout);
    };
}