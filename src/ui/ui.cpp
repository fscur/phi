#include <precompiled.h>
#include "ui.h"
#include "control.h"
#include "text.h"

namespace phi
{   
    ui::ui(camera* camera, gl* gl, float w, float h) :
        _gl(gl),
        _controls(vector<node*>()),
        _camera(camera),
        _w(w),
        _h(h)
    {
        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);

        auto camTransform = _camera->getTransform();

        camTransform->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        camTransform->setDirection(vec3(0.0f, 0.0f, -1.0f));
    }

    ui::~ui()
    {
        for (auto controls : _controls)
            safeDelete(controls);
    }

    void ui::update()
    {
    }

    void ui::render()
    {
    }

    void ui::add(node* n)
    {
        _controls.push_back(n);
    }
}