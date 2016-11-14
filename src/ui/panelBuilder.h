#include <phi.h>
#include <core/node.h>
#include "uiApi.h"

namespace phi
{
    class panelBuilder
    {
    public:
        UI_API static panelBuilder newPanel();

    public:
        UI_API panelBuilder withPosition(vec3 position);
        UI_API panelBuilder withOrientation(quat orientation);
        UI_API panelBuilder withSize(vec3 size);
        UI_API panelBuilder withControlColor(float r, float g, float b, float a);
        UI_API panelBuilder withGlassyTransparency();
        UI_API node* build();

    private:
        panelBuilder(node* node) :
            _panel(node)
        {
        }

    private:
        node* _panel;
    };
}