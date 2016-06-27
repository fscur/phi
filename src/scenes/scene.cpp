#include <precompiled.h>
#include "scene.h"

#include <core\boxCollider.h>
#include <core\obb.h>

#include "sceneId.h"

namespace phi
{
    scene::scene(gl* gl, float w, float h) :
        _gl(gl),
        _pipeline(new pipeline(gl, w, h)),
        _camera(new camera("mainCamera", w, h, 0.1f, 1000.0f, PI_OVER_4)),
        _sceneRoot(new node("sceneRoot")),
        _w(w),
        _h(h),
        _nodeTokens(map<node*, nodeEventTokens*>()),
        _physicsWorld(new physicsWorld()),
        _selectedNodes(vector<node*>())
    {
        trackNode(_sceneRoot);

        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        for (auto pair : _nodeTokens)
        {
            pair.first->childAdded.unassign(pair.second->childAdded);
            pair.first->childRemoved.unassign(pair.second->childRemoved);
            pair.first->transformChanged.unassign(pair.second->transformChanged);

            safeDelete(pair.second);
        }

        _nodeTokens.clear();

        safeDelete(_physicsWorld);
        safeDelete(_sceneRoot);
        safeDelete(_pipeline);
    }

    //TODO: keep a list of dirty nodes to prevent traversing and some hard to fix buggy bugs
    //TODO: after this fix, grouping should work correctly and nodes pointer should be deleted on deleteSceneObjectCommand
    void scene::trackNode(node* node)
    {
        auto childAddedToken = node->childAdded.assign(std::bind(&scene::nodeChildAdded, this, std::placeholders::_1));
        auto childRemovedToken = node->childRemoved.assign(std::bind(&scene::nodeChildRemoved, this, std::placeholders::_1));
        auto transformChangedToken = node->transformChanged.assign(std::bind(&scene::nodeTransformChanged, this, std::placeholders::_1));
        auto selectionChangedToken = node->selectionChanged.assign(std::bind(&scene::nodeSelectionChanged, this, std::placeholders::_1));

        _nodeTokens[node] = new nodeEventTokens(
            childAddedToken,
            childRemovedToken,
            transformChangedToken,
            selectionChangedToken);
    }

    void scene::untrackNode(node* node)
    {
        node->childAdded.unassign(_nodeTokens[node]->childAdded);
        node->childRemoved.unassign(_nodeTokens[node]->childRemoved);
        node->transformChanged.unassign(_nodeTokens[node]->transformChanged);
        node->selectionChanged.unassign(_nodeTokens[node]->selectionChanged);

        safeDelete(_nodeTokens[node]);
        _nodeTokens.erase(node);
    }

    void scene::nodeChildAdded(node* addedChild)
    {
        addedChild->traverse([&](phi::node* node)
        {
            trackNode(node);

            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                sceneId::setNextId(mesh);
                _pipeline->add(mesh, node->getTransform()->getModelMatrix());

                auto aabb = mesh->geometry->aabb;
                auto collider = new boxCollider("obbCollider", aabb->center, vec3(aabb->halfWidth, aabb->halfHeight, aabb->halfDepth));
                node->addComponent(collider);

                _physicsWorld->addCollider(collider);
                //_pipeline->_renderer->getBoxColliderRenderPass()->addCollider(collider);
            }
        });
    }

    void scene::nodeChildRemoved(node* removedChild)
    {
        removedChild->traverse([&](node* node)
        {
            untrackNode(node);

            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
                _pipeline->remove(mesh);

            auto collider = node->getComponent<boxCollider>();
            if (collider)
            {
                _physicsWorld->removeCollider(collider);
                //_pipeline->_renderer->getBoxColliderRenderPass()->removeCollider(collider);
            }
        });
    }

    void scene::nodeTransformChanged(node* changedNode)
    {
        auto mesh = changedNode->getComponent<phi::mesh>();
        if (mesh)
            _pipeline->updateTranformBuffer(mesh, changedNode->getTransform()->getModelMatrix());
    }

    void scene::nodeSelectionChanged(node* node)
    {
        auto isSelected = node->getIsSelected();
        if (isSelected)
            _selectedNodes.push_back(node);
        else
            phi::removeIfContains(_selectedNodes, node);

        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
            _pipeline->updateSelectionBuffer(mesh, isSelected);
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
            1, 1); // What a shitty shit, right ? R: I've seen worse

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