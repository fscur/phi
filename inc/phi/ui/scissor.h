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

        scissor() :
            x(0), y(0), width(0), height(0)
        {
        }

        scissor(float x, float y, float width, float height) :
            x(x), y(y), width(width), height(height)
        {
        }

        scissor intersection(scissor b)
        {
            scissor intersection;
            intersection.x = glm::max(x, b.x);
            intersection.y = glm::max(y, b.y);
            intersection.width = glm::min(x + width, b.x + b.width) - intersection.x;
            intersection.height = glm::min(y + height, b.y + b.height) - intersection.y;

            if (intersection.x + intersection.width > intersection.x &&
                intersection.y + intersection.height > intersection.y)
                return intersection;

            scissor();
        }

        bool intersectsWith(glm::vec2 point)
        {
            return
                point.x >= x && point.x <= x + width &&
                point.y >= y && point.y <= y + height;
        }
    };
}

#endif