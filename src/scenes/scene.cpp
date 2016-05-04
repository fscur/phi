#include <precompiled.h>
#include "scene.h"

#include <rendering\model.h>

#include "sceneId.h"

namespace phi
{
    scene::scene(gl* gl, float w, float h) :
        _gl(gl),
        _renderer(new renderer(gl, w, h)),
        _pipeline(new pipeline(gl)),
        _camera(new camera("mainCamera", w, h, 0.1f, 1000.0f, PI_OVER_4)),
        _objects(vector<node*>()),
        _meshesIds(map<int, mesh*>()),
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
        auto pixels = _renderer->getGBufferRenderPass()->getFramebuffer()->readPixels(
            _renderer->getGBufferRenderPass()->rt3,
            static_cast<GLint>(mouseX),
            static_cast<GLint>(_h - mouseY),
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
        return _renderer->getDefaultFramebuffer()->getZBufferValue(x, y);
    }
}