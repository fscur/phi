#ifndef _PHI_CAROUSEL_ITEM_H_
#define _PHI_CAROUSEL_ITEM_H_

#include "button.h"
#include "carouselItemEventArgs.h"
#include "quadRenderer2D.h"

namespace phi
{
    class carouselItem : public button
    {
    private:
        std::string _name;

    public:
        UI_API carouselItem(std::string name, size<GLuint> viewportSize) : _name(name), button(viewportSize) {}

        UI_API std::string getName() { return _name; }
    };
}

#endif