#include "scene.h"

#include <rendering\model.h>

#include <algorithm>

namespace phi
{
    scene::scene(camera* camera) :
        _camera(camera)
    {
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
    }

    void scene::setSize(sizef size)
    {
        _size = size;
        _activeCamera->getFrustum()->setAspect((float) _size.w / (float) _size.h);
    }

    void scene::addToRenderList(object3D* object)
    {
        if (object->getType() == object3D::objectType::MESH)
        {
            auto m = static_cast<mesh*>(object);
            auto mat = m->material;
            auto geom = m->geometry;

            _renderList[geom].push_back(m);
        }

        auto children = object->getChildren();

        for (object3D* child : children)
            addToRenderList(child);
    }

    void scene::traverseTree(object3D* node, std::function<void(object3D*)> callback)
    {
        callback(node);

        for (auto child : node->getChildren())
            traverseTree(child, callback);
    }

    void scene::traverseTreeMeshes(object3D* node, std::function<void(mesh*)> callback)
    {
        traverseTree(node, [&](object3D* n)
        {
            if (n->getType() == phi::object3D::objectType::MESH)
            {
                auto m = static_cast<mesh*>(n);
                callback(m);
            }
        });
    }

    void scene::add(object3D* object)
    {
        _objects.push_back(object);

        traverseTreeMeshes(object, [&](mesh* m)
        {
            auto geometry = m->geometry;
            _loadedGeometries[geometry]++;
        });

        addToRenderList(object);
    }

    void scene::remove(object3D* object)
    {
        auto position = std::find(_objects.begin(), _objects.end(), object);

        traverseTreeMeshes(object, [&](mesh* m)
        {
            auto geometry = m->geometry;
            _loadedGeometries[geometry]--;
            if (_loadedGeometries[geometry] <= 0)
            {
                _loadedGeometries.erase(geometry);
            }
        });

        if (position != _objects.end())
            _objects.erase(position);
    }
}