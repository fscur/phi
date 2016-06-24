#include <precompiled.h>
#include "buttonBuilder.h"

#include <core\clickComponent.h>

#include "control.h"
#include "text.h"

namespace phi
{
    buttonBuilder buttonBuilder::withPosition(vec3 position)
    {
        _button->setPosition(position);
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
        auto label = new phi::text("controlText");

        label->setControl(control);
        label->setFont(fontsManager::defaultFont);
        label->setText(text);

        _button->addComponent(label);

        //TODO: this event should be unassigned.
        control->getOnSizeChanged()->assign([=](vec3 size)
        {
            _button->setSize(size);
        });
        
        _button->setSize(control->getSize());

        return *this;
    }

    buttonBuilder buttonBuilder::withAction(std::function<void(node*)> action)
    {
        _button->getComponent<clickComponent>()->addOnClick(action);
        return *this;
    }

    node* buttonBuilder::build()
    {
        return _button;
    }

    buttonBuilder phi::buttonBuilder::newButton()
    {
        auto node = new phi::node("button");
        auto control = new phi::control("buttonControl");
        auto clickComponent = new phi::clickComponent("buttonClick");

        node->addComponent(control);
        node->addComponent(clickComponent);

        return buttonBuilder(node);
    }
}
