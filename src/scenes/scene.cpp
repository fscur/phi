#include "scene.h"

#include <rendering\model.h>

#include <algorithm>

namespace phi
{
    scene::scene(phi::gl* gl, size_t w, size_t h) :
        w(w),
        h(h),
        _gl(gl),
        camera(new phi::camera(0.1f, 1000.0f, vec2(w, h), glm::half_pi<float>()))
    {
        _pipeline = new phi::pipeline(gl);
        _renderer = new phi::renderer(gl, w, h);
    }

    scene::~scene()
    {
        for (auto object : _objects)
            safeDelete(object);
    }

    void scene::update()
    {
        for (auto obj : _objects)
            obj->update();

        camera->update();

        auto frameUniformBlock = phi::frameUniformBlock();
        frameUniformBlock.p = camera->getProjectionMatrix();
        frameUniformBlock.v = camera->getViewMatrix();
        frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;
        frameUniformBlock.ip = glm::inverse(frameUniformBlock.p);

        _pipeline->updateFrameUniformBlock(frameUniformBlock);

        _renderer->gBufferPass->batches = _pipeline->batches;
        _renderer->update();
    }

    void scene::render()
    {
        _renderer->render();
    }

    void scene::resize(size_t w, size_t h)
    {
        camera->setResolution(vec2(w, h));
    }

    void scene::add(object3D* object)
    {
        _objects.push_back(object);
        _pipeline->add(object);
    }

    void scene::remove(object3D* object)
    {
        auto position = std::find(_objects.begin(), _objects.end(), object);

        if (position != _objects.end())
            _objects.erase(position);
    }
}