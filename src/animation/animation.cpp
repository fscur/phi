#include <precompiled.h>
#include "animation.h"
#include <core\notImplementedException.h>
#include <core\time.h>

namespace phi
{
    animation::animation(std::function<double(double)> easingFunction) :
        _isRunning(false),
        _duration(0.0),
        _elapsed(0.0),
        _easingFunction(easingFunction)
    {
    }

    animation::animation(const animation& original) :
        _isRunning(original._isRunning),
        _duration(original._duration),
        _elapsed(original._elapsed),
        _easingFunction(original._easingFunction)
    {

    }

    animation::~animation()
    {
    }

    void animation::animate()
    {
        if (!_isRunning)
            return;

        _elapsed += time::deltaSeconds;

        auto percent = glm::min(_elapsed / _duration, 1.0);

        update(_easingFunction(percent));

        if (_elapsed > _duration)
        {
            if (_animationEndedCallback)
                _animationEndedCallback();

            stop();
        }
    }

    void animation::start(double duration, std::function<void(void)> animationEndedCallback)
    {
        _duration = duration;
        _elapsed = 0.0;
        _isRunning = true;
        _animationEndedCallback = animationEndedCallback;
    }

    void animation::start(double duration)
    {
        start(duration, {});
    }

    void animation::stop()
    {
        _isRunning = false;
    }
}