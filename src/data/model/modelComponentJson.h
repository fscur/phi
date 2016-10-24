#pragma once
#include <phi.h>

#include "componentJson.h"

namespace phi
{
    struct modelComponentJson : public componentJson
    {
        string thumbnailFileName;
        string animatedThumbnailFileName;
    };
}