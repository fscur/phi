#include <precompiled.h>
#include <common/mouseInteractionComponent.h>
#include <ui/layoutTransform.h>
#include <ui/controlMouseHoverAnimatable.h>
#include "switchControl.h"
#include "switchControlBuilder.h"

namespace phi
{
    switchControlBuilder switchControlBuilder::newSwitchControl()
    {
        return switchControlBuilder();
    }

    switchControlBuilder::switchControlBuilder()
    {
        _switchControlNode = new phi::node("switchControl", new layoutTransform());
        auto control = new phi::control();
        control->setColor(color(1.0f, 1.0f, 1.0f, 0.5f));
        control->setIsGlassy(true);
        auto switchControl = new phi::switchControl();
        _switchControlNode->addComponent(control);
        _switchControlNode->addComponent(switchControl);

        auto handle = new phi::node("handle", new layoutTransform());
        auto handleControl = new phi::control();
        handleControl->setColor(color(33.0f / 255.0f, 150.0f / 255.0f, 243.0f / 255.0f, 0.6f));
        auto handleAnimator = new animator();
        handle->addComponent(handleControl);
        handle->addComponent(handleAnimator);

        _optionANode = createOptionNode("optionA");
        _optionBNode = createOptionNode("optionB");

        _switchControlNode->addChild(handle);
        _switchControlNode->addChild(_optionANode);
        _switchControlNode->addChild(_optionBNode);

        switchControl->setHandle(handle);
        switchControl->setOptionA(_optionANode);
        switchControl->setOptionB(_optionBNode);
    }

    switchControlBuilder switchControlBuilder::withPosition(vec3 position)
    {
        _switchControlNode->setPosition(position);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOrientation(quat orientation)
    {
        _switchControlNode->setOrientation(orientation);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withSize(vec3 size)
    {
        _switchControlNode->setSize(size);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOptionAImage(image* image)
    {
        _optionANode->getComponent<control>()->setImage(image);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOptionBImage(image* image)
    {
        _optionBNode->getComponent<control>()->setImage(image);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOptionACallback(action<> callback)
    {
        _switchControlNode->getComponent<switchControl>()->setOptionACallback(callback);
        return *this;
    }

    switchControlBuilder switchControlBuilder::withOptionBCallback(action<> callback)
    {
        _switchControlNode->getComponent<switchControl>()->setOptionBCallback(callback);
        return *this;
    }

    node* switchControlBuilder::build()
    {
        return _switchControlNode;
    }

    node* switchControlBuilder::createOptionNode(string name)
    {
        auto optionNode = new phi::node(name, new layoutTransform());

        auto control = new phi::control();
        auto animator = new phi::animator();
        auto interaction = new mouseInteractionComponent();

        auto hoverAnimation = new controlMouseHoverAnimatable(control, animator);
        hoverAnimation->setHoverColor(color(1.0f, 1.0f, 1.0f, 0.15f));

        interaction->setMouseHoverAnimatable(hoverAnimation);

        optionNode->addComponent(control);
        optionNode->addComponent(animator);
        optionNode->addComponent(interaction);

        return optionNode;
    }
}