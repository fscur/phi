#pragma once
#include <phi.h>
#include "animationApi.h"
#include <core\time.h>
#include <core\notImplementedException.h>
#include <core\eventHandler.h>

namespace phi
{
    class animation
    {
    public:
        virtual ~animation();

    protected:
        bool _isRunning;
        double _duration;
        double _elapsed;
        std::function<double(double)> _easingFunction;
        std::function<void(void)> _animationEndedCallback;

    protected:
        virtual void update(double t) = 0;
        
        void start(double duration, std::function<void(void)> animationEndedCallback);
        void start(double duration);

    public:
        ANIMATION_API animation(std::function<double(double)> easingFunction);
        ANIMATION_API animation(const animation& original);

        ANIMATION_API void animate();
        ANIMATION_API void stop();

        ANIMATION_API virtual animation* clone() = 0;
        bool isRunning() const { return _isRunning; }
    };
}