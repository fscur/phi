#include <precompiled.h>

#include <core\time.h>

#include "floatAnimator.h"

namespace phi
{
    vector<floatAnimation*> floatAnimator::_animations;
    vector<floatAnimation*> floatAnimator::_toCancelAnimations;
    double floatAnimator::_lastUpdateMilliseconds = 0.0;

    void floatAnimator::animateFloat(floatAnimation* animation)
    {
        _animations.push_back(animation);
    }

    void floatAnimator::cancelAnimation(floatAnimation* animation)
    {
        if (phi::contains(_animations, animation))
            _toCancelAnimations.push_back(animation);
    }

    void floatAnimator::update()
    {
        for (auto animation : _toCancelAnimations)
        {
            removeIfContains(_animations, animation);
            safeDelete(animation);
        }
        _toCancelAnimations.clear();

        double currentMilliseconds = time::totalSeconds * 1000;

        for (unsigned int i = 0; i < _animations.size(); i++)
        {
            floatAnimation* animation = _animations[i];
            if (animation->getDelayElapsed() < animation->getDelayMilliseconds())
            {
                animation->setDelayElapsed(animation->getDelayElapsed() + currentMilliseconds - _lastUpdateMilliseconds);
                continue;
            }

            animation->setElapsed(animation->getElapsed() + currentMilliseconds - _lastUpdateMilliseconds);

            float from = animation->getFrom();
            float to = animation->getTo();

            float percent = (float)animation->getElapsed() / (float)animation->getMilliseconds();
            percent = glm::clamp(percent, 0.0f, 1.0f);
            float diff = animation->getEasingFunction()(percent);

            auto value = from + (to - from) * diff;

            if (animation->getElapsed() > (float)animation->getMilliseconds())
            {
                auto callback = animation->getCallback();
                if (callback != nullptr)
                    callback(value);

                auto endCallback = animation->getEndCallback();
                if (endCallback)
                    endCallback();

                safeDelete(animation);
                _animations.erase(_animations.begin() + i);
                i--;
            }
            else
            {
                auto callback = animation->getCallback();
                if (callback != nullptr)
                    callback(value);
            }
        }

        _lastUpdateMilliseconds = currentMilliseconds;
    }
}