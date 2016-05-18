#include <precompiled.h>
#include "scene.h"
#include "boxCollider.h"

#include <core\obb.h>
#include <rendering\model.h>

namespace phi
{
    scene::scene(gl* gl, size_t w, size_t h) :
        _gl(gl),
        w(w),
        h(h)
    {
        _pipeline = new pipeline(gl);
        renderer = new phi::renderer(gl, w, h);

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
        _objects.push_back(node);
        _pipeline->add(node);

        phi::aabb* aabb = new phi::aabb();
        node->traverse
        (
            [&aabb](phi::node* n)
        {
            auto meshComponent = n->getComponent<phi::mesh>();
            if (meshComponent)
            {
                auto added = new phi::aabb(phi::aabb::add(*meshComponent->geometry->aabb, *aabb));
                safeDelete(aabb);
                aabb = added;
            }
        }
        );

        if (aabb->min == aabb->max)
        {
            safeDelete(aabb);
            return;
        }

        auto transform = node->getTransform();
        auto pos = transform->getPosition();
        auto scale = transform->getLocalSize();

        auto collider = new boxCollider("obbCollider", aabb->center, vec3(aabb->halfWidth * scale.x, aabb->halfHeight * scale.y, aabb->halfDepth * scale.z));
        node->addComponent(collider);

        colliderNodes.push_back(node);

        safeDelete(aabb);
    }

    void scene::remove(node* node)
    {
        auto position = std::find(_objects.begin(), _objects.end(), node);

        if (position != _objects.end())
            _objects.erase(position);
    }

    inline float scene::getZBufferValue(int x, int y)
    {
        return renderer->defaultFramebuffer->getZBufferValue(x, y);
    }
}