#include <precompiled.h>
#include "ui.h"
#include "controlRenderer.h"
#include "textRenderer.h"

namespace phi
{   
    ui::ui(renderer* renderer, gl* gl, float w, float h) :
        _gl(gl),
        _controls(vector<node*>()),
        _camera(nullptr),
        _renderer(nullptr),
        _w(w),
        _h(h)
    {
        _camera = new camera("uiCamera", w, h, 0.1f, 10000.0f, PI_OVER_4);

        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);

        auto camTransform = _camera->getTransform();

        camTransform->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        camTransform->setDirection(vec3(0.0f, 0.0f, -1.0f));
        //_camera->orbit(vec3(0.0f), camTransform->getRight(), camTransform->getUp(), 0.4f, 0.0f);
        _renderer = new uiRenderer(renderer, gl, _camera);
    }

    ui::~ui()
    {
        for (auto controls : _controls)
            safeDelete(controls);

        safeDelete(_renderer);
    }

    // float t = 0.0f;

    void ui::update()
    {
        //_camera->orbit(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, glm::cos(t) * 0.1f);
        //_camera->moveTo(vec3(0.0, 0.0, glm::abs(glm::cos(t)) * 500.0f));
        _renderer->update();

        //t += 0.0005f;
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

    node* ui::newLabel(wstring text, vec3 position)
    {
        auto node = new phi::node();

        auto resizeNode = [node](vec3 size)
        {
            node->setSize(size);
        };

        auto controlRenderer = new phi::controlRenderer("labelControlRenderer");

        auto textRenderer = new phi::textRenderer("labelTextRenderer");
        textRenderer->setControlRenderer(controlRenderer);
        textRenderer->setFont(_gl->defaultFont);
        textRenderer->setText(text);

        node->addComponent(controlRenderer);
        node->addComponent(textRenderer);

        node->setPosition(position);
        node->setSize(controlRenderer->getSize());

        controlRenderer->getOnSizeChanged()->assign(resizeNode); //TODO: this event should be unassigned.

        return node;
    }
}