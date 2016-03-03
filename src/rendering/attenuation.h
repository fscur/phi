#pragma once
#include <phi.h>

namespace phi
{
    struct attenuation
    {
    public:
        float constant;
        float linear;
        float exponential;

    public:
        attenuation(float constant = 0.0f, float linear = 0.0f, float exponential = 1.0f) :
            constant(constant), linear(linear), exponential(exponential) {}

        ~attenuation() {}
    };
}