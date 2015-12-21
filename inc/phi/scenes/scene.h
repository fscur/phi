#ifndef _PHI_SCENE_H_
#define _PHI_SCENE_H_

#include "scenes.h"

#include <phi\core\globals.h>
#include <phi\core\mathUtils.h>
#include <phi\rendering\shaderManager.h>
#include <phi\rendering\material.h>
#include <phi\rendering\mesh.h>
#include <phi\rendering\geometryRenderer.h>
#include <phi\rendering\camera.h>

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
        std::map<material*, std::map<geometry*, std::vector<mesh*>>> _renderList;
    private:
        void addToRenderList(object3D* object);


    public:
        SCENES_API scene(camera* camera);
        SCENES_API ~scene();
        
        SCENES_API std::vector<object3D*> getObjects() { return _objects; }
        SCENES_API std::map<material*, std::map<geometry*, std::vector<mesh*>>>& getRenderList() { return _renderList; }

        SCENES_API camera* getCamera() const { return _camera; }
        SCENES_API void setCamera(camera* value) { _camera = value; }

        SCENES_API void update();
        SCENES_API void add(object3D* object);
        SCENES_API void remove(object3D* object);
    };
}
#endif