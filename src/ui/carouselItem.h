#pragma once
#include <precompiled.h>

#include <rendering\quadRenderer2D.h>

#include "button.h"
#include "carouselItemEventArgs.h"

namespace phi
{
    class carouselItem : public button
    {
    private:
        std::string _name;

    public:
        UI_API carouselItem(std::string name, sizef viewportSize) : _name(name), button(viewportSize) {}

        UI_API std::string getName() { return _name; }
    };
}