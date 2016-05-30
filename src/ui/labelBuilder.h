#pragma once
#include <phi.h>
#include "uiApi.h"

#include <core\node.h>

#include <rendering\gl.h>
#include <rendering\font.h>

namespace phi
{
    class labelBuilder
    {
    private:
        phi::node* _label;

    private:
        labelBuilder(phi::node* node) :
            _label(node)
        {
        }

    public:
        UI_API labelBuilder withPosition(vec3 position);
        UI_API labelBuilder withFont(phi::font* font);
        UI_API labelBuilder withControlColor(float r, float g, float b, float a);
        UI_API labelBuilder withTextColor(float r, float g, float b, float a);
        UI_API node* build();

    public:
        UI_API static labelBuilder newLabel(wstring text, phi::gl* gl);
    };
}