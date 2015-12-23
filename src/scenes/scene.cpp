#include <phi\rendering\model.h>
#include <phi\scenes\scene.h>

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
            auto m = static_cast<mesh*>(object);
            auto mat = m->getMaterial();
            auto geom = m->getGeometry();

            _renderList[mat][geom].push_back(m);
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
        traverseTree(node, [&] (object3D* n)
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

        traverseTreeMeshes(object, [&] (mesh* m)
        {
            auto mat = m->getMaterial();

            auto loadTex = [&](texture* tex)
            {
                if (tex)
                {
                    tex->loadOnGpu();
                    _loadedTextures[tex]++;
                }
            };

            loadTex(mat->getDiffuseTexture());
            loadTex(mat->getNormalTexture());
            loadTex(mat->getSpecularTexture());
            loadTex(mat->getEmissiveTexture());

            auto geometry = m->getGeometry();
            geometry->loadToGpu();
            _loadedGeometries[geometry]++;
        });

        addToRenderList(object);
    }

    void scene::remove(object3D* object)
    {
        auto position = std::find(_objects.begin(), _objects.end(), object);

        traverseTreeMeshes(object, [&](mesh* m)
        {
            auto mat = m->getMaterial();

            auto unloadTex = [&](texture* tex)
            {
                if (tex)
                {
                    _loadedTextures[tex]--;
                    if (_loadedTextures[tex] <= 0)
                    {
                        tex->releaseFromGpu();
                        _loadedTextures.erase(tex);
                    }
                }
            };

            unloadTex(mat->getDiffuseTexture());
            unloadTex(mat->getNormalTexture());
            unloadTex(mat->getSpecularTexture());
            unloadTex(mat->getEmissiveTexture());

            auto geometry = m->getGeometry();
            _loadedGeometries[geometry]--;
            if (_loadedGeometries[geometry] <= 0)
            {
                geometry->releaseFromGpu();
                _loadedGeometries.erase(geometry);
            }
        });

        if (position != _objects.end())
            _objects.erase(position);
    }
}