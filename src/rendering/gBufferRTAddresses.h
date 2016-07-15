#pragma once
#include <phi.h>

namespace phi
{
    struct gBufferRTAddresses
    {
        int rt0Unit;
        int rt1Unit;
        int rt2Unit;
        int depthUnit;
        float rt0Page;
        float rt1Page;
        float rt2Page;
        float depthPage;
    };
}