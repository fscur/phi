#include "floatAnimator.h"
#include "clock.h"
#include "glm/gtx/constants.hpp"
#include <string>
#include "globals.h"

namespace phi
{
    std::vector<floatAnimation*> floatAnimator::_animations;
    int floatAnimator::_lastUpdateMilliseconds = 0;

    void floatAnimator::animateFloat(float* value, float to, int milliseconds)
    {
        animateFloat(new floatAnimation(value, to, milliseconds));
    }

    void floatAnimator::animateFloat(floatAnimation* animation)
    {
        _animations.push_back(animation);
    }

    void floatAnimator::update()
    {
        int currentMilliseconds = clock::millisecondsElapsed;

        for (unsigned int i = 0; i < _animations.size(); i++)
        {
            floatAnimation* animation = _animations[i];
            float* value = animation->getValue();
            float from = animation->getFrom();
            float to = animation->getTo();

            float percent = (float)animation->getElapsed() / (float)animation->getMilliseconds();
            float diff = sin(percent * glm::pi<float>() * 0.5f);

            *value = from + (to - from) * diff;

            animation->setElapsed(animation->getElapsed() + currentMilliseconds - _lastUpdateMilliseconds);

            if (percent >= 1.0f)
            {
                DELETE(animation);
                _animations.erase(_animations.begin() + i);
                i--;
            }
        }

        _lastUpdateMilliseconds = currentMilliseconds;
    }
}