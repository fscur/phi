#ifndef _PHI_HUD_H_
#define _PHI_HUD_H_

#include "control.h"
#include "size.h"

namespace phi
{
    class hud
    {
    private:
        std::vector<control*> _controls;
		size<GLuint> _viewportSize;

    public:
		UI_API size<GLuint> getViewportSize() { return _viewportSize; }
		UI_API void setViewportSize(size<GLuint> value);

        UI_API void addControl(control* control);
        UI_API void render();
    };
}

#endif