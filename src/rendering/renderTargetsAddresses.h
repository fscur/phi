#pragma once
#include <phi.h>

namespace phi
{
    struct renderTargetsAddresses
    {
        int rt0Unit;
        int rt1Unit;
        int rt2Unit;
        int rt3Unit;
        int depthUnit;
        int pad0;
        int pad1;
        int pad2;
        float rt0Page;
        float rt1Page;
        float rt2Page;
        float rt3Page;
        float depthPage;
        float pad3;
        float pad4;
        float pad5;
    };

}