#include <phi.h>
#include <core/node.h>
#include <core/image.h>
#include "control.h"
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
        UI_API switchControlBuilder withOptionAImage(image* texture);
        UI_API switchControlBuilder withOptionBImage(image* texture);
        UI_API switchControlBuilder withOptionACallback(action<> callback);
        UI_API switchControlBuilder withOptionBCallback(action<> callback);
        UI_API node* build();

    private:
        switchControlBuilder();

        node* createOptionNode(string name);

    private:
        node* _switchControlNode;
        node* _optionANode;
        node* _optionBNode;
    };
}