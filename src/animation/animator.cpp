#include <precompiled.h>
#include "animator.h"

namespace phi
{
    animator::animator() :
        component(componentType::ANIMATOR),
        _animations(vector<animation*>())
    {
    }

    animator::animator(const animator& original) :
        component(componentType::ANIMATOR)
    {
        _unused(original);

        for (auto animation : _animations)
            addAnimation(animation->clone());
    }

    animator::~animator()
    {
    }

    void animator::addAnimation(animation* animation)
    {
        _animations.push_back(animation);
    }

    void animator::animate()
    {
        for (auto& animation : _animations)
            animation->animate();
    }

    component* animator::clone() const
    {
        auto original = static_cast<const animator*>(this);
        return new animator(*original);
    }
}