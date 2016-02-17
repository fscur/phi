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

    void scene::add(object3D* object)
    {
        _objects.push_back(object);
    }

    void scene::remove(object3D* object)
    {
        auto position = std::find(_objects.begin(), _objects.end(), object);

        if (position != _objects.end())
            _objects.erase(position);
    }
}