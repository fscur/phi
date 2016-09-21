#pragma once
#include <phi.h>

#include <core/node.h>

#include <rendering/fontsManager.h>
#include <rendering/font.h>

#include "uiApi.h"

namespace phi
{
    class buttonBuilder
    {
    private:
        phi::node* _button;

    private:
        buttonBuilder(phi::node* node) :
            _button(node)
        {
        }

    public:
        UI_API buttonBuilder withPosition(vec3 position);
        UI_API buttonBuilder withOrientation(quat orientation);
        UI_API buttonBuilder withFont(phi::font* font);
        UI_API buttonBuilder withControlColor(float r, float g, float b, float a);
        UI_API buttonBuilder withTextColor(float r, float g, float b, float a);
        UI_API buttonBuilder withText(wstring text);
        UI_API buttonBuilder asBillboard();
        UI_API buttonBuilder withAction(std::function<void(node*)> action);
        UI_API node* build();

    public:
        UI_API static buttonBuilder newButton();
    };
}