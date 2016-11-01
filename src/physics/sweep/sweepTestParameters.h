#pragma once

#include <phi.h>

namespace phi
{
    namespace sweep
    {
        class sweepTestParameters
        {
        public:
            sweepTestParameters() :
                distance(0.0f),
                direction(vec3()),
                disregardDivergentNormals(true),
                maximumHits(32u),
                inflation(0.0f),
                checkPenetration(false)
            {
            }

            float distance;
            vec3 direction;
            bool disregardDivergentNormals;
            uint32_t maximumHits;
            float inflation;
            bool checkPenetration;
        };
    }
}