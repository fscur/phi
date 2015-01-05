#include "colorAnimator.h"

namespace phi
{
    std::unordered_map<color*, colorAnimation> colorAnimator::_animations;

    void colorAnimator::animateColor(color* colorFrom, color colorTo)
    {
        animateColor(colorAnimation(colorFrom, colorTo));
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

            float r = glm::sign(rDiff) * 0.05f; // TODO: use time interval instead of fixed value per frame
            float g = glm::sign(gDiff) * 0.05f;
            float b = glm::sign(bDiff) * 0.05f;
            float a = glm::sign(aDiff) * 0.05f;

            colorFrom->r += r < 0 ? glm::max(r, rDiff) : glm::min(r, rDiff);
            colorFrom->g += g < 0 ? glm::max(g, gDiff) : glm::min(g, gDiff);
            colorFrom->b += b < 0 ? glm::max(b, bDiff) : glm::min(b, bDiff);
            colorFrom->a += a < 0 ? glm::max(a, aDiff) : glm::min(a, aDiff);

            if (colorFrom->r == colorTo.r && 
                colorFrom->g == colorTo.g &&
                colorFrom->b == colorTo.b &&
                colorFrom->a == colorTo.a)
                _animations.erase(i++);
            else
                i++;
        }
    }
}