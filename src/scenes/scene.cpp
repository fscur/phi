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
        _sceneRoot(new node("sceneRoot")),
        _w(w),
        _h(h)
    {
        trackNode(_sceneRoot);

        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        safeDelete(_sceneRoot);
        safeDelete(_pipeline);
    }

    void scene::trackNode(node* node)
    {
        node->childAdded += std::bind(&scene::nodeChildAdded, this, std::placeholders::_1);
        node->childRemoved += std::bind(&scene::nodeChildRemoved, this, std::placeholders::_1);
        node->transformChanged += std::bind(&scene::nodeTransformChanged, this, std::placeholders::_1);
    }

    void scene::nodeChildAdded(node* addedChild)
    {
        addedChild->traverse([&](phi::node* node)
        {
            trackNode(node);

            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                mesh->selectionChanged += std::bind(&scene::meshSelectionChanged, this, std::placeholders::_1);

                sceneId::setNextId(mesh);

                renderInstance instance = { 0 };
                instance.mesh = mesh;
                instance.modelMatrix = node->getTransform()->getModelMatrix();
                instance.materialId = -1; //gambis

                _pipeline->add(instance);
            }
        });
    }

    void scene::nodeChildRemoved(node* removedChild)
    {
        removedChild->traverse([&](node* node) 
        {
            //unnassign events
            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                _pipeline->remove(mesh);
            }
        });
    }

    void scene::nodeTransformChanged(node* changedNode)
    {
        changedNode->traverseNodesContaining<mesh>([&](node* node, mesh* mesh)
        {
            _pipeline->updateTranformBuffer(mesh, node->getTransform()->getModelMatrix());
        });
    }

    void scene::meshSelectionChanged(mesh* mesh)
    {
        _pipeline->updateSelectionBuffer(mesh);
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
        _sceneRoot->addChild(node);
    }

    void scene::remove(node* node)
    {
        _sceneRoot->removeChild(node);
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