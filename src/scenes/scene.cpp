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
        traverseTree(node, [&](object3D* n)
        {
            if (n->getType() == phi::object3D::objectType::MESH)
            {
                auto m = static_cast<mesh*>(n);
                callback(m);
            }
        });
    }

    void scene::addTextureToArray(texture* tex)
    {
        auto texWidth = tex->getWidth();
        auto texHeight = tex->getHeight();
        auto it = find_if(_textureArrays.begin(), _textureArrays.end(), [&](textureArray* texArray)
        {
            auto arraySize = texArray->getSize();
            return texWidth == arraySize.w &&
                texHeight == arraySize.h;
        });

        textureArray* sameSizeArray = nullptr;
        if (it != _textureArrays.end())
            sameSizeArray = it[0];
        else
        {
            auto textureUnit = (GLint)_textureArrays.size();
            _textureArrayUnits.push_back(textureUnit);

            sameSizeArray = new textureArray(sizeui(texWidth, texHeight, TEXTURE_ARRAY_DEPTH), textureUnit);
            _textureArrays.push_back(sameSizeArray);
            sameSizeArray->loadOnGpu();
        }

        if (!sameSizeArray->hasTexture(tex))
        {
            sameSizeArray->add(tex);
            auto page = sameSizeArray->getTextureIndex(tex);
            _textureStorageDatas[tex].arrayIndex = (GLint)(find(_textureArrays.begin(), _textureArrays.end(), sameSizeArray) - _textureArrays.begin());
            _textureStorageDatas[tex].pageIndex = page;
        }
    }

    void scene::add(object3D* object)
    {
        _objects.push_back(object);

        traverseTreeMeshes(object, [&](mesh* m)
        {
            auto mat = m->getMaterial();

            auto loadTex = [&](texture* tex)
            {
                if (tex)
                    addTextureToArray(tex);
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
                    for (auto texArray : _textureArrays)
                    {
                        if (texArray->hasTexture(tex))
                        {
                            texArray->remove(tex);
                            break;
                        }
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