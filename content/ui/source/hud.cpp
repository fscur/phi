#include "hud.h"

namespace phi
{
    void hud::addControl(control* control)
    {
        _controls.push_back(control);
    }

    void hud::render()
    {
        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->render();
        }
    }

	void hud::setViewportSize(size<GLuint> value)
    {
        _viewportSize = value;

        for (unsigned int i = 0; i < _controls.size(); i++)
        {
            control* control = _controls[i];
            control->setViewportSize(_viewportSize);
        }
    }
}