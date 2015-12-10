#include <phi/ui/scissor.h>
#include <phi/core/globals.h>

namespace phi
{
    scissor::scissor() :
        x(0), y(0), w(0), h(0)
    {
    }

    scissor::scissor(float x, float y, float width, float height) :
        x(x), y(y), w(width), h(height)
    {
    }

    scissor scissor::intersection(scissor b)
    {
        scissor intersection;
        intersection.x = glm::max(x, b.x);
        intersection.y = glm::max(y, b.y);
        intersection.w = glm::min(x + w, b.x + b.w) - intersection.x;
        intersection.h = glm::min(y + h, b.y + b.h) - intersection.y;

        if (intersection.x + intersection.w > intersection.x &&
            intersection.y + intersection.h > intersection.y)
            return intersection;

        return scissor();
    }

    bool scissor::intersectsWith(vec2 point)
    {
        return
            point.x >= x && point.x <= x + w &&
            point.y >= y && point.y <= y + h;
    }
}