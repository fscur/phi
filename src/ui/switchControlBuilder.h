#include <phi.h>
#include <core/node.h>
#include "uiApi.h"

namespace phi
{
    class switchControlBuilder
    {
    public:
        UI_API static switchControlBuilder newSwitchControl();

    public:
        UI_API switchControlBuilder withPosition(vec3 position);
        UI_API switchControlBuilder withOrientation(quat orientation);
        UI_API switchControlBuilder withSize(vec3 size);
        UI_API node* build();

    private:
        switchControlBuilder(phi::node* node) :
            _switchControl(node)
        {
        }

    private:
        node* _switchControl;
    };
}