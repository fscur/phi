#include <precompiled.h>
#include "buttonBuilder.h"

#include <animation/animator.h>
#include <animation/floatAnimation.h>

#include <common/mouseInteractionComponent.h>

#include "controlMouseClickAnimatable.h"
#include "controlMouseHoverAnimatable.h"
#include "control.h"
#include "text.h"

namespace phi
{
    buttonBuilder buttonBuilder::withPosition(vec3 position)
    {
        _button->setPosition(position);
        return *this;
    }

    buttonBuilder buttonBuilder::withOrientation(quat orientation)
    {
        _button->setOrientation(orientation);
        return *this;
    }

    buttonBuilder buttonBuilder::withFont(font * font)
    {
        _button->getComponent<text>()->setFont(font);
        return *this;
    }

    buttonBuilder buttonBuilder::withControlColor(float r, float g, float b, float a)
    {
        _button->getComponent<control>()->setColor(color::fromRGBA(r, g, b, a));
        return *this;
    }

    buttonBuilder buttonBuilder::withTextColor(float r, float g, float b, float a)
    {
        _button->getComponent<text>()->setColor(color::fromRGBA(r, g, b, a));
        return *this;
    }

    buttonBuilder buttonBuilder::withText(wstring text)
    {
        auto control = _button->getComponent<phi::control>();
        auto label = new phi::text();

        label->setControl(control);
        label->setFont(fontsManager::defaultFont);
        label->setText(text);

        _button->addComponent(label);

        //TODO: this event should be unassigned.
        control->getOnSizeChanged()->assign([=](vec3 size)
        {
            assert(size.x != 0 && size.y != 0 && size.z != 0);
            _button->setSize(size);
        });

        _button->setSize(control->getSize());

        return *this;
    }

    buttonBuilder buttonBuilder::asBillboard()
    {
        _button->getComponent<control>()->setIsBillboard(true);
        _button->getComponent<text>()->setIsBillboard(true);
        return *this;
    }

    buttonBuilder buttonBuilder::withAction(std::function<void(node*)> action)
    {
        _button->getComponent<mouseInteractionComponent>()->addOnMouseUp(action);
        return *this;
    }

    node* buttonBuilder::build()
    {
        return _button;
    }

    buttonBuilder phi::buttonBuilder::newButton()
    {
        auto node = new phi::node("button");
        auto animator = new phi::animator();
        auto control = new phi::control();
        auto mouseInteraction = new phi::mouseInteractionComponent();

        mouseInteraction->setMouseClickAnimatable(new controlMouseClickAnimatable(control, animator));
        mouseInteraction->setMouseHoverAnimatable(new controlMouseHoverAnimatable(control, animator));

        node->addComponent(control);
        node->addComponent(mouseInteraction);
        node->addComponent(animator);

        return buttonBuilder(node);
    }
}
