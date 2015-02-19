#include "colorAnimator.h"
#include "clock.h"

namespace phi
{
    std::unordered_map<color*, colorAnimation> colorAnimator::_animations;
    int colorAnimator::_lastUpdateMilliseconds = 0;

    void colorAnimator::animateColor(color* colorFrom, color colorTo, int milliseconds)
    {
        animateColor(colorAnimation(colorFrom, colorTo, milliseconds));
    }

    void colorAnimator::animateColor(colorAnimation animation)
    {
        auto it = _animations.find(animation.getColorFrom());
        if(it != _animations.end())
            it->second = animation;
        else
            _animations.insert(std::make_pair(animation.getColorFrom(),animation));
    }

    void colorAnimator::update()
    {
        int currentMilliseconds = clock::millisecondsElapsed;

        std::unordered_map<color*, colorAnimation>::iterator i = _animations.begin();
        while (i != _animations.end())
        {
            colorAnimation animation = i->second;
            color* colorFrom = animation.getColorFrom();
            color colorTo = animation.getColorTo();

            float rDiff = colorTo.r - colorFrom->r;
            float gDiff = colorTo.g - colorFrom->g;
            float bDiff = colorTo.b - colorFrom->b;
            float aDiff = colorTo.a - colorFrom->a;

            float percent = (float)(currentMilliseconds - _lastUpdateMilliseconds) / (float)animation.getMilliseconds();
            float r = glm::sign(rDiff) * percent;
            float g = glm::sign(gDiff) * percent;
            float b = glm::sign(bDiff) * percent;
            float a = glm::sign(aDiff) * percent;

            colorFrom->r += rDiff < 0 ? glm::max(r, rDiff) : glm::min(r, rDiff);
            colorFrom->g += gDiff < 0 ? glm::max(g, gDiff) : glm::min(g, gDiff);
            colorFrom->b += bDiff < 0 ? glm::max(b, bDiff) : glm::min(b, bDiff);
            colorFrom->a += aDiff < 0 ? glm::max(a, aDiff) : glm::min(a, aDiff);

            animation.setElapsed(animation.getElapsed() + currentMilliseconds - _lastUpdateMilliseconds);

            if (colorFrom->r == colorTo.r && 
                colorFrom->g == colorTo.g &&
                colorFrom->b == colorTo.b &&
                colorFrom->a == colorTo.a)
                _animations.erase(i++);
            else
                i++;
        }

        _lastUpdateMilliseconds = currentMilliseconds;
    }
}