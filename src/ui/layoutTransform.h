#pragma once

#include <phi.h>
#include <core/transform.h>
#include "uiApi.h"

namespace phi
{
    class layoutTransform :
        public transform
    {
    public:
        layoutTransform();

        UI_API void updateData() override;
    };
}