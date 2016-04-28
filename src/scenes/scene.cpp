#include <precompiled.h>
#include "scene.h"

#include <rendering\model.h>

namespace phi
{
    scene::scene(gl* gl, float w, float h) :
        _gl(gl),
        _objects(vector<node*>()),
        _renderer(new renderer(gl, w, h)),
        _pipeline(new pipeline(gl)),
        _camera(new camera("mainCamera", w, h, 0.1f, 1000.0f, PI_OVER_4)),
        _w(w),
        _h(h)
    {
        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        for (auto object : _objects)
            safeDelete(object);

        safeDelete(_pipeline);
        safeDelete(_renderer);
    }

    void scene::update()
    {
        auto frameUniformBlock = phi::frameUniformBlock();
        frameUniformBlock.p = _camera->getProjectionMatrix();
        frameUniformBlock.v = _camera->getViewMatrix();
        frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;
        frameUniformBlock.ip = glm::inverse(frameUniformBlock.p);

        _pipeline->updateFrameUniformBlock(frameUniformBlock);

        //TODO: possible design flaw ? anyway.. try to remove this slow copy assignment
        _renderer->getGBufferRenderPass()->setBatches(_pipeline->batches);
        _renderer->update();
    }

    void scene::render()
    {
        _renderer->render();
    }

    void scene::resize(float w, float h)
    {
        _camera->setWidth(w);
        _camera->setHeight(h);
    }

    void scene::add(node* n)
    {
        _objects.push_back(n);
        _pipeline->add(n);
    }

    void scene::remove(node* n)
    {
        auto position = std::find(_objects.begin(), _objects.end(), n);

        if (position != _objects.end())
            _objects.erase(position);
    }

    inline float scene::getZBufferValue(int x, int y)
    {
        return _renderer->getDefaultFramebuffer()->getZBufferValue(x, y);
    }
}