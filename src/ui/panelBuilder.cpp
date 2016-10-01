#include <precompiled.h>
#include <ui/control.h>
#include <ui/layoutTransform.h>
#include "panelBuilder.h"

namespace phi
{
    panelBuilder panelBuilder::newPanel()
    {
        auto node = new phi::node("panel", new layoutTransform());
        auto control = new phi::control();
        node->addComponent(control);

        return panelBuilder(node);
    }

    panelBuilder panelBuilder::withPosition(vec3 position)
    {
        _panel->setPosition(position);
        return *this;
    }

    panelBuilder panelBuilder::withOrientation(quat orientation)
    {
        _panel->setOrientation(orientation);
        return *this;
    }

    panelBuilder panelBuilder::withSize(vec3 size)
    {
        _panel->setSize(size);
        return *this;
    }

    panelBuilder panelBuilder::withControlColor(float r, float g, float b, float a)
    {
        _panel->getComponent<control>()->setColor(color::fromRGBA(r, g, b, a));
        return *this;
    }

    panelBuilder panelBuilder::withGlassyTransparency()
    {
        _panel->getComponent<control>()->setIsGlassy(true);
        return *this;
    }

    node* panelBuilder::build()
    {
        return _panel;
    }
}