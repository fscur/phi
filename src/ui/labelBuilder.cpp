#include <precompiled.h>

#include <ui/layoutTransform.h>

#include "control.h"
#include "labelBuilder.h"
#include "text.h"

namespace phi
{
    labelBuilder phi::labelBuilder::newLabel(wstring text)
    {
        auto node = new phi::node("label", new layoutTransform());
        auto control = new phi::control();
        auto label = new phi::text();
        node->addComponent(control);
        node->addComponent(label);

        label->setControl(control);
        label->setFont(fontsManager::defaultFont);
        label->setText(text);

        //TODO: this event should be unassigned.
        control->getOnSizeChanged()->assign([node](vec3 size)
        {
            node->setSize(size);
        });
        node->setSize(control->getSize());

        return labelBuilder(node);
    }

    labelBuilder labelBuilder::withPosition(vec3 position)
    {
        _label->setPosition(position);
        return *this;
    }

    labelBuilder labelBuilder::withFont(font * font)
    {
        _label->getComponent<text>()->setFont(font);
        return *this;
    }

    labelBuilder labelBuilder::withGlassyLook()
    {
        _label->getComponent<control>()->setIsGlassy(true);
        return *this;
    }

    labelBuilder labelBuilder::withControlColor(float r, float g, float b, float a)
    {
        _label->getComponent<control>()->setColor(color::fromRGBA(r, g, b, a));
        return *this;
    }

    labelBuilder labelBuilder::withTextColor(float r, float g, float b, float a)
    {
        _label->getComponent<text>()->setColor(color::fromRGBA(r, g, b, a));
        return *this;
    }

    node* labelBuilder::build()
    {
        return _label;
    }
}