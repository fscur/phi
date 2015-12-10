#include <phi/scenes/scene.h>

#include <algorithm>

namespace phi
{
    scene::scene(camera* camera) :
        _camera(camera)
    {
    }

    scene::~scene()
    {
        for(auto object : _objects)
            safeDelete(object);
    }

    void scene::update()
    {
    }

    void scene::addToRenderList(object3D* object)
    {
        if (object->getType() == object3D::objectType::MESH)
        {
            auto m = dynamic_cast<mesh*>(object);
            auto mat = m->getMaterial();
            auto geom = m->getGeometry();

            _renderList[mat].push_back(m);
        }

        auto children = object->getChildren();

        for (object3D* child : children)
            addToRenderList(child);
    }

    void scene::add(object3D* object)
    {
        _objects.push_back(object);

        addToRenderList(object);
    }

    void scene::remove(object3D* object)
    {
        auto position = std::find(_objects.begin(), _objects.end(), object);

        if (position != _objects.end())
            _objects.erase(position--);
    }
}