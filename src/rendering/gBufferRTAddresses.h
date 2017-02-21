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
        int pickingUnit;
        int pad0;
        int pad1;
        int pad2;
        float rt0Page;
        float rt1Page;
        float rt2Page;
        float depthPage;
        float pickingPage;
        float pad3;
        float pad4;
        float pad5;
    };
}