#pragma once
#include <phi.h>
#include <core\time.h>
#include <core\notImplementedException.h>
#include "iAnimation.h"

namespace phi
{
    template <typename TYPE>
    class animation :
        public iAnimation
    {
    protected:
        TYPE* _value;
        TYPE* _from; 
        TYPE* _to;

        bool _isAnimating;
        double _duration;
        double _elapsed;
        std::function<double(double)> _easingFunction;
        virtual void update(double t) = 0;

    public:
        animation(
            TYPE* value, 
            std::function<double(double)> easingFunction) :
            _value(value),
            _from(nullptr),
            _to(nullptr),
            _isAnimating(false),
            _duration(0.0),
            _elapsed(0.0),
            _easingFunction(easingFunction)
        {
        }

        animation(const animation& original) :
            _isAnimating(original._isAnimating),
            _duration(original._duration),
            _elapsed(original._elapsed),
            _easingFunction(original._easingFunction)
        {
        
        }

        ~animation()
        {
        }

        void start(TYPE* from, TYPE* to, double duration)
        {
            _from = from;
            _to = to;
            _duration = duration;
            _elapsed = 0.0;
            _isAnimating = true;
        }

        void animate()
        {
            if (!_isAnimating)
                return;

            _elapsed += time::deltaSeconds;

            auto percent = glm::min(_elapsed / _duration, 1.0);

            update(_easingFunction(percent));

            if (_elapsed > _duration)
                stop();
        }

        void stop()
        {
            _isAnimating = false;
        }
    };
}