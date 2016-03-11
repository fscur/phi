#include <precompiled.h>
#include "scene.h"

#include <rendering\model.h>

namespace phi
{
    scene::scene(phi::gl* gl, size_t w, size_t h) :
        w(w),
        h(h),
        _gl(gl)
    {
        _pipeline = new phi::pipeline(gl);
        _renderer = new phi::renderer(gl, w, h);

        camera = new phi::camera("mainCamera", 0.1f, 1000.0f, vec2(w, h), glm::half_pi<float>());
        auto cameraNode = new node();
        cameraNode->addComponent(camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        for (auto object : _objects)
            safeDelete(object);
    }

    void scene::update()
    {
        auto frameUniformBlock = phi::frameUniformBlock();
        frameUniformBlock.p = camera->getProjectionMatrix();
        frameUniformBlock.v = camera->getViewMatrix();
        frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;
        frameUniformBlock.ip = glm::inverse(frameUniformBlock.p);

        _pipeline->updateFrameUniformBlock(frameUniformBlock);

        _renderer->gBufferPass->batches = _pipeline->batches;
        _renderer->update();

        camera->update();
    }

    void scene::render()
    {
        _renderer->render();
    }

    void scene::resize(size_t w, size_t h)
    {
        camera->setResolution(vec2(w, h));
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

    inline float scene::getZBufferValue(phi::ivec2 mousePos)
    {
        auto pos = phi::ivec2(mousePos.x, h - mousePos.y);
        auto z = _renderer->defaultFramebuffer->getZBufferValue(pos);
        phi::debug("z buffer value: " + std::to_string(z));
        return z;
    }
    
}