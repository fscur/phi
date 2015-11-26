#include <phi/ui/scissor.h>
#include <phi/core/globals.h>

namespace phi
{
    scissor::scissor() :
        x(0), y(0), width(0), height(0)
    {
    }

    scissor::scissor(float x, float y, float width, float height) :
        x(x), y(y), width(width), height(height)
    {
    }

    scissor scissor::intersection(scissor b)
    {
        scissor intersection;
        intersection.x = glm::max(x, b.x);
        intersection.y = glm::max(y, b.y);
        intersection.width = glm::min(x + width, b.x + b.width) - intersection.x;
        intersection.height = glm::min(y + height, b.y + b.height) - intersection.y;

        if (intersection.x + intersection.width > intersection.x &&
            intersection.y + intersection.height > intersection.y)
            return intersection;

        return scissor();
    }

    bool scissor::intersectsWith(glm::vec2 point)
    {
        return
            point.x >= x && point.x <= x + width &&
            point.y >= y && point.y <= y + height;
    }
}