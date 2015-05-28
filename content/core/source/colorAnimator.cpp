#include "colorAnimator.h"
#include "clock.h"

namespace phi
{
    std::unordered_map<color*, colorAnimation*> colorAnimator::_animations;
    int colorAnimator::_lastUpdateMilliseconds = 0;

    void colorAnimator::animateColor(color* colorFrom, color colorTo, int milliseconds)
    {
        animateColor(new colorAnimation(colorFrom, colorTo, milliseconds));
    }

    void colorAnimator::animateColor(colorAnimation* animation)
    {
        auto it = _animations.find(animation->getColorFromVar());
        if(it != _animations.end())
            it->second = animation;
        else
            _animations.insert(std::make_pair(animation->getColorFromVar(), animation));
    }

    void colorAnimator::update()
    {
        int currentMilliseconds = clock::millisecondsElapsed;

        std::unordered_map<color*, colorAnimation*>::iterator i = _animations.begin();
        while (i != _animations.end())
        {
            colorAnimation* animation = i->second;
            color colorFrom = animation->getColorFrom();
            color* colorFromVar = animation->getColorFromVar();
            color colorTo = animation->getColorTo();

            float rDiff = colorTo.r - colorFrom.r;
            float gDiff = colorTo.g - colorFrom.g;
            float bDiff = colorTo.b - colorFrom.b;
            float aDiff = colorTo.a - colorFrom.a;

            animation->setElapsed(animation->getElapsed() + currentMilliseconds - _lastUpdateMilliseconds);

            float percent = (float)animation->getElapsed() / (float)animation->getMilliseconds();

            colorFromVar->r = colorFrom.r + rDiff * percent;
            colorFromVar->g = colorFrom.g + gDiff * percent;
            colorFromVar->b = colorFrom.b + bDiff * percent;
            colorFromVar->a = colorFrom.a + aDiff * percent;

            if (percent >= 1.0f)
                _animations.erase(i++);
            else
                i++;
        }

        _lastUpdateMilliseconds = currentMilliseconds;
    }
}