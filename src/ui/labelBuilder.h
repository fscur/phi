#pragma once
#include <phi.h>
#include "uiApi.h"

#include <core\node.h>

#include <rendering\fontsManager.h>
#include <rendering\font.h>

namespace phi
{
    class labelBuilder
    {
    public:
        UI_API static labelBuilder newLabel(wstring text);

    public:
        UI_API labelBuilder withPosition(vec3 position);
        UI_API labelBuilder withFont(phi::font* font);
        UI_API labelBuilder withGlassyLook();
        UI_API labelBuilder withControlColor(float r, float g, float b, float a);
        UI_API labelBuilder withTextColor(float r, float g, float b, float a);
        UI_API node* build();

    private:
        labelBuilder(phi::node* node) :
            _label(node)
        {
        }

    private:
        phi::node* _label;
    };
}