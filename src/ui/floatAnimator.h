#ifndef _PHI_FLOAT_ANIMATOR_H_
#define _PHI_FLOAT_ANIMATOR_H_

#include "ui.h"
#include "floatAnimation.h"

#include <unordered_map>

namespace phi
{
    class floatAnimator
    {
    private:
        static std::vector<floatAnimation*> _animations;
        static int _lastUpdateMilliseconds;

    public:
        UI_API static void animateFloat(float* floatFrom, float floatTo, int milliseconds);
        UI_API static void animateFloat(floatAnimation* animation);
        UI_API static void update();
    };
}

#endif