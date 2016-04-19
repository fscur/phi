#include <precompiled.h>
#include "ui.h"
#include "controlRenderer.h"
#include "textRenderer.h"

namespace phi
{   
    ui::ui(gl* gl, float w, float h) :
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

        camTransform->setLocalPosition(vec3(0.0f, 0.0f, 1000.0f));
        camTransform->setDirection(vec3(0.0f, 0.0f, -1.0f));
        _camera->orbit(vec3(0.0f), camTransform->getRight(), camTransform->getUp(), 0.1f, 0.4f);
        _renderer = new uiRenderer(gl, _camera);
    }

    ui::~ui()
    {
        for (auto controls : _controls)
            safeDelete(controls);

        safeDelete(_camera);
        safeDelete(_renderer);
    }
    float t = 0.0f;

    void ui::update()
    {
        auto camTransform = _camera->getTransform();
        auto c = glm::cos(t);
        t += 0.01f;

        _camera->orbit(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, c * 0.1f);
        //camTransform->translate(vec3(c * 1.0f, 0.0f, 0.0f));

        _renderer->update();
    }

    void ui::render()
    {
        _renderer->render();
    }

    void ui::add(node* n)
    {
        _controls.push_back(n);
        //traverse node tree and find controlRenderers and textRenderers and add them
        /*
            if node has controlRenderer
                _renderer->addControlRenderer(controlRenderer);
                _renderer->addTextRenderer(textRenderer);
        */

        _renderer->add(n);
    }

    node* ui::newLabel(wstring text, vec3 position, vec3 size)
    {
        auto node = new phi::node();

        node->addComponent(new controlRenderer("labelControlRenderer"));
        auto textRenderer = new phi::textRenderer("labelTextRenderer");
        textRenderer->setText(text);
        node->addComponent(textRenderer);

        node->setPosition(position);
        node->setSize(size);

        return node;
    }
}