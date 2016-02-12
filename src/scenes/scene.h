#ifndef _PHI_SCENE_H_
#define _PHI_SCENE_H_

#include "scenes.h"

#include <core\globals.h>
#include <core\mathUtils.h>
#include <rendering\mesh.h>
#include <rendering\camera.h>

#include <map>
#include <typeinfo>
#include <mutex>

namespace phi
{
    class scene
    {
    public:
        struct renderData
        {
            geometry* geometry;
            mat4 modelMatrix;
        };

    private:

        camera *_camera;
        std::vector<object3D*> _objects;
        std::map<geometry*, std::vector<mesh*>> _renderList;
        std::map<geometry*, int> _loadedGeometries;

        sizef _size;
        camera *_activeCamera;

    private:
        void addToRenderList(object3D* object);
        void traverseTree(object3D* node, std::function<void(object3D*)> callback);
        void traverseTreeMeshes(object3D* node, std::function<void(mesh*)> callback);

    public:
        SCENES_API scene(camera* camera);
        SCENES_API ~scene();
        
        SCENES_API std::vector<object3D*> getObjects() { return _objects; }
        SCENES_API std::map<geometry*, std::vector<mesh*>>& getRenderList() { return _renderList; }

        SCENES_API inline camera* getCamera() const { return _camera; }
        SCENES_API inline void setCamera(camera* value) { _camera = value; }

        SCENES_API inline void setSize(sizef size);
        SCENES_API inline void setActiveCamera(camera* value) { _activeCamera = value; }

        SCENES_API void update();

        SCENES_API void add(object3D* object);
        SCENES_API void remove(object3D* object);
    };
}
#endif