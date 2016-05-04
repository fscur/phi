#include <precompiled.h>
#include "scene.h"

#include <rendering\model.h>

#include "sceneId.h"

namespace phi
{
    scene::scene(gl* gl, size_t w, size_t h) :
        _gl(gl),
        w(w),
        h(h),
        _pipeline(new pipeline(gl)),
        renderer(new phi::renderer(gl, w, h))
    {
        camera = new phi::camera(
            "mainCamera",
            static_cast<float>(w),
            static_cast<float>(h),
            0.1f,
            1000.0f,
            glm::half_pi<float>() * 0.5f);

        auto cameraNode = new node();
        cameraNode->addComponent(camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        for (auto object : _objects)
            safeDelete(object);

        safeDelete(_pipeline);
        safeDelete(renderer);
    }

    void scene::update()
    {
        auto frameUniformBlock = phi::frameUniformBlock();
        frameUniformBlock.p = camera->getProjectionMatrix();
        frameUniformBlock.v = camera->getViewMatrix();
        frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;
        frameUniformBlock.ip = glm::inverse(frameUniformBlock.p);

        _pipeline->updateFrameUniformBlock(frameUniformBlock);

        //TODO: possible design flaw ? anyway.. try to remove this slow copy assignment
        renderer->gBufferPass->batches = _pipeline->batches;
        renderer->update();
    }

    void scene::render()
    {
        renderer->render();
    }

    void scene::resize(size_t w, size_t h)
    {
        camera->setWidth(static_cast<float>(w));
        camera->setHeight(static_cast<float>(h));
    }

    void scene::add(node* node)
    {
        node->traverse<mesh>([&](mesh* mesh)
        {
            auto id = sceneId::next();
            mesh->setId(id);
            _meshesIds[id] = mesh;
        });

        _objects.push_back(node);
        _pipeline->add(node);
    }

    void scene::remove(node* node)
    {
        auto position = std::find(_objects.begin(), _objects.end(), node);

        if (position != _objects.end())
            _objects.erase(position);

        node->traverse<mesh>([&](mesh* mesh)
        {
            _meshesIds.erase(mesh->getId());
        });
    }

    mesh* scene::pick(int mouseX, int mouseY)
    {
        auto pixels = renderer->gBufferPass->framebuffer->readPixels(
            renderer->gBufferPass->rt3,
            static_cast<GLint>(mouseX),
            static_cast<GLint>(h - mouseY),
            1, 1);

        auto r = static_cast<int>(pixels.r);
        auto g = static_cast<int>(pixels.g) << 8;
        auto b = static_cast<int>(pixels.b) << 16;

        auto id = r | g | b;

        mesh* mesh = nullptr;
        if (id)
            mesh = _meshesIds[id];

        return mesh;
    }

    inline float scene::getZBufferValue(int x, int y)
    {
        return renderer->defaultFramebuffer->getZBufferValue(x, y);
    }
}