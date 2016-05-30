#include <precompiled.h>
#include "ui.h"
#include "control.h"
#include "text.h"

namespace phi
{   
    ui::ui(camera* camera, renderer* renderer, gl* gl, float w, float h) :
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
        _renderer = new uiRenderer(renderer, gl, _camera);
    }

    ui::~ui()
    {
        for (auto controls : _controls)
            safeDelete(controls);

        safeDelete(_renderer);
    }

    void ui::update()
    {
        _renderer->update();
    }

    void ui::render()
    {
        _renderer->render();
    }

    void ui::add(node* n)
    {
        _controls.push_back(n);
        _renderer->add(n);
    }
}