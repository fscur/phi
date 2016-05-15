#include <precompiled.h>
#include "scene.h"
#include <rendering\materialGpuData.h>
#include <rendering\renderInstance.h>

#include "sceneId.h"

namespace phi
{
    scene::scene(gl* gl, float w, float h) :
        _gl(gl),
        _pipeline(new pipeline(gl, w, h)),
        _camera(new camera("mainCamera", w, h, 0.1f, 1000.0f, PI_OVER_4)),
        _objects(vector<node*>()),
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
    }

    void scene::update()
    {
        auto frameUniform = frameUniformBlock();
        frameUniform.p = _camera->getProjectionMatrix();
        frameUniform.v = _camera->getViewMatrix();
        frameUniform.vp = frameUniform.p * frameUniform.v;
        frameUniform.ip = glm::inverse(frameUniform.p);

        _pipeline->update(frameUniform);
    }

    void scene::render()
    {
        _pipeline->render();
    }

    void scene::resize(float w, float h)
    {
        _camera->setWidth(w);
        _camera->setHeight(h);
    }

    void scene::add(node* node)
    {
        node->traverseNodesContaining<mesh>([&](phi::node* currentNode, mesh* mesh)
        {
            sceneId::setNextId(mesh);

            renderInstance instance = { 0 };
            instance.mesh = mesh;
            instance.modelMatrix = currentNode->getTransform()->getModelMatrix();
            instance.materialId = -1; //gambis

            _pipeline->add(instance);
        });

        _objects.push_back(node);
    }

    void scene::remove(node* node)
    {
        phi::removeIfContains(_objects, node);

        node->traverse<mesh>([&](mesh* mesh)
        {
            sceneId::removeMeshId(mesh);
        });
    }

    mesh* scene::pick(int mouseX, int mouseY)
    {
        auto pixels = _pipeline->_renderer->getGBufferFramebuffer()->readPixels(
            _pipeline->_renderer->rt3,
            static_cast<GLint>(mouseX),
            static_cast<GLint>(_h - mouseY),
            1, 1); // What a shitty shit, right ?

        auto r = static_cast<int>(pixels.r);
        auto g = static_cast<int>(pixels.g) << 8;
        auto b = static_cast<int>(pixels.b) << 16;

        auto id = r | g | b;

        mesh* mesh = nullptr;
        if (id)
            mesh = sceneId::getMesh(id);

        return mesh;
    }

    inline float scene::getZBufferValue(int x, int y)
    {
        return _pipeline->_renderer->getDefaultFramebuffer()->getZBufferValue(x, y);
    }
}