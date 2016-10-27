#pragma once
#include <phi.h>

#include <core/guid.h>
#include <core/image.h>

namespace phi
{
    class iTextureRepository
    {
    public:
        virtual image* getById(guid guid) = 0;
    };
}