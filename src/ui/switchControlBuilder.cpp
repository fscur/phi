#include <precompiled.h>
#include <common/mouseInteractionComponent.h>
#include <ui/control.h>
#include <ui/layoutTransform.h>
#include "switchControl.h"
#include "switchControlBuilder.h"

namespace phi
{
    switchControlBuilder switchControlBuilder::newSwitchControl()
    {
        auto node = new phi::node("switchControl", new layoutTransform());
        auto control = new phi::control();
        node->addComponent(control);
        auto interaction = new mouseInteractionComponent();
        node->addComponent(interaction);
        auto switchControl = new phi::switchControl();
        node->addComponent(switchControl);

        auto handle = new phi::node("handle", new layoutTransform());
        auto handleControl = new phi::control();
        handleControl->setColor(color(1.0f, 1.0f, 1.0f, 0.5f));
        handle->addComponent(handleControl);

        auto optionA = new phi::node("optionA", new layoutTransform());
        auto optionAControl = new phi::control();
        optionAControl->setIsGlassy(true);
        optionAControl->setColor(color(1.0f, 1.0f, 1.0f, 0.5f));
        optionA->addComponent(optionAControl);

        auto optionB = new phi::node("optionB", new layoutTransform());
        auto optionBControl = new phi::control();
        optionBControl->setIsGlassy(true);
        optionBControl->setColor(color(1.0f, 1.0f, 1.0f, 0.5f));
        optionB->addComponent(optionBControl);

        node->addChild(optionA);
        node->addChild(optionB);
        node->addChild(handle);

        switchControl->setHandleControl(handleControl);
        switchControl->setOptionAControl(optionAControl);
        switchControl->setOptionBControl(optionBControl);

        return switchControlBuilder(node);
    }

    switchControlBuilder switchControlBuilder::withPosition(vec3 position)
    {
        _switchControl->setPosition(position);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOrientation(quat orientation)
    {
        _switchControl->setOrientation(orientation);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withSize(vec3 size)
    {
        _switchControl->setSize(size);
        return *this;
    }

    node* switchControlBuilder::build()
    {
        return _switchControl;
    }
}