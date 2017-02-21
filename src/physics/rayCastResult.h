#pragma once
#include <phi.h>
#include "rayCastHit.h"

namespace phi
{
    struct rayCastResult
    {
        vector<rayCastHit> hits;

        rayCastResult(vector<rayCastHit>&& hits) :
            hits(hits) 
        {}

        rayCastResult() :
            hits(vector<rayCastHit>())
        {}
    };
}