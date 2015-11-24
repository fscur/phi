#ifndef _PHI_SCISSOR_H_
#define _PHI_SCISSOR_H_

#include <glm/glm.hpp>

namespace phi
{
    struct scissor
    {
    public:
        float x;
        float y;
        float width;
        float height;

        scissor::scissor();
        scissor(float x, float y, float width, float height);
        scissor intersection(scissor b);
        bool intersectsWith(glm::vec2 point);
    };
}

#endif