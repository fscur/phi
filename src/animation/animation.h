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
    protected:
        bool _isAnimating;
        double _duration;
        double _elapsed;
        std::function<double(double)> _easingFunction;
        eventHandler<animation*> _animationEnded;

    protected:
        virtual void update(double t) = 0;
        virtual ~animation();
        
        void start(double duration);

    public:
        ANIMATION_API animation(std::function<double(double)> easingFunction);
        ANIMATION_API animation(const animation& original);

        ANIMATION_API void animate();
        ANIMATION_API void stop();

        ANIMATION_API virtual animation* clone() = 0;

        eventHandler<animation*>* getAnimationEnded() { return &_animationEnded; }
        bool getIsAnimating() const { return _isAnimating; }
    };
}