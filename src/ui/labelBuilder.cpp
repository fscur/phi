#include <precompiled.h>
#include "labelBuilder.h"

#include "control.h"
#include "text.h"

namespace phi
{
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

    labelBuilder phi::labelBuilder::newLabel(wstring text, gl* gl)
    {
        auto node = new phi::node("label");
        auto control = new phi::control("labelControl");
        auto label = new phi::text("labelText");
        node->addComponent(control);
        node->addComponent(label);

        label->setControl(control);
        label->setFont(gl->defaultFont);
        label->setText(text);

        //TODO: this event should be unassigned.
        control->getOnSizeChanged()->assign([node](vec3 size)
        {
            node->setSize(size);
        });
        node->setSize(control->getSize());

        return labelBuilder(node);
    }
}
