#ifndef _PHI_SCENE_MESH_
#define _PHI_SCENE_MESH_

#include <phi/rendering/material.h>
#include <phi/rendering/mesh.h>

#include "phi/scenes/scenes.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>

namespace phi
{
    class sceneMesh
    {
    private:
        bool _isSelected;
        mesh* _mesh;
        std::string _materialName;
        material* _material;
        btGImpactMeshShape* _bulletShape;

    public:
        SCENES_API sceneMesh(mesh* mesh);

        SCENES_API bool getIsSelected() const { return _isSelected; }
        SCENES_API mesh* getMesh() const { return _mesh; }
        SCENES_API material* getMaterial() const { return _material; }
        SCENES_API btGImpactMeshShape* getBulletShape() const { return _bulletShape; }

        SCENES_API void setIsSelected(bool value) { _isSelected = value; }
        SCENES_API void setMesh(mesh* value) { _mesh = value; }
        SCENES_API void setMaterial(material* value);
    };
}

#endif