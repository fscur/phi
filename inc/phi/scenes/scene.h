#ifndef _PHI_SCENE_H_
#define _PHI_SCENE_H_

#include "phi/core/globals.h"
#include "phi/core/mathUtils.h"

#include "phi/rendering/shaderManager.h"
#include "phi/rendering/material.h"
#include "phi/rendering/meshRenderer.h"

#include "phi/scenes/scenes.h"
#include "phi/scenes/sceneObject.h"
#include "phi/scenes/camera.h"
#include "phi/scenes/directionalLight.h"
#include "phi/scenes/pointLight.h"
#include "phi/scenes/spotLight.h"
#include "phi/scenes/sceneObjectEventArgs.h"

#include <typeinfo>
#include <mutex>

namespace phi
{
    class scene
    {
    private:
        GLuint _allObjectsCount;
        GLuint _visibleObjectsCount;
        GLuint _staticObjectsCount;
        GLuint _dynamicObjectsCount;
        GLuint _changedObjectsCount;

        size<GLuint> _size;
        float _deltaTime;
        color _backColor;
        color _ambientColor;
        bool _lightChanged;
        aabb* _aabb;

        camera *_activeCamera;

        std::vector<sceneObject*>* _allObjects;
        std::map<GLuint, sceneObject*>* _staticObjects;
        std::map<GLuint, sceneObject*>* _dynamicObjects;
        std::vector<sceneObject*>* _visibleObjects;
        std::vector<sceneObject*>* _selectedObjects;
        std::vector<object3D*> _changedObjects;
        std::mutex _changedObjectsMutex;

        std::vector<directionalLight*>* _directionalLights;
        std::vector<pointLight*>* _pointLights;
        std::vector<spotLight*>* _spotLights;

        std::vector<camera*>* _cameras;

        eventHandler<sceneObjectEventArgs>* _selectedSceneObjectChanged;
        eventHandler<eventArgs>* _staticObjectsChanged;

        unsigned int _sceneObjectsIds;

    private:
        void sceneObjectChanged(phi::object3DEventArgs e);
        void lightChanged(phi::object3DEventArgs e);
        void sceneObjectIsSelectedChanged(phi::sceneObjectEventArgs e);
        void staticObjectsChanged(phi::eventArgs e);
        void addSceneObjectAabb(sceneObject* sceneObject);
        void updateAabb();

    public:
        SCENES_API scene();
        SCENES_API ~scene();

        SCENES_API camera* getActiveCamera() const { return _activeCamera; }
        SCENES_API color getBackColor() const { return _backColor; }
        SCENES_API color getAmbientColor() const { return _ambientColor; }
        SCENES_API std::vector<sceneObject*>* getAllObjects() { return _allObjects; }
        SCENES_API std::map<GLuint, sceneObject*>* getStaticObjects() { return _staticObjects; }
        SCENES_API std::map<GLuint, sceneObject*>* getDynamicObjects() { return _dynamicObjects; }
        SCENES_API std::vector<sceneObject*>* getVisibleObjects() const { return _visibleObjects; }
        SCENES_API GLuint getVisibleObjectsCount() const { return _visibleObjectsCount; }
        SCENES_API GLuint getAllObjectsCount() const { return _allObjectsCount; }
        SCENES_API std::vector<directionalLight*>* getDirectionalLights() const { return _directionalLights; }
        SCENES_API std::vector<pointLight*>* getPointLights() const { return _pointLights; }
        SCENES_API std::vector<spotLight*>* getSpotLights() const { return _spotLights; }
        SCENES_API eventHandler<sceneObjectEventArgs>* getSelectedSceneObjectChanged() const { return _selectedSceneObjectChanged; }
        SCENES_API eventHandler<eventArgs>* getStaticObjectsChanged() const { return _staticObjectsChanged; }
        SCENES_API aabb* getAabb() const { return _aabb; }
        
        SCENES_API void setSize(size<GLuint> size);
        SCENES_API void setActiveCamera(camera* value) { _activeCamera = value; }
        SCENES_API void setBackColor(color value) { _backColor = value; }
        SCENES_API void setAmbientLightColor(color value) { _ambientColor = value; }
        SCENES_API void setDeltaTime(float value) { _deltaTime = value; }

        SCENES_API void input();
        SCENES_API void update();

        SCENES_API void add(sceneObject* sceneObject);
        SCENES_API void add(directionalLight* directionalLight);
        SCENES_API void add(pointLight* pointLight);
        SCENES_API void add(spotLight* spotLight);
        SCENES_API void add(camera* camera);

        SCENES_API std::vector<sceneObject*>* getSelected();
        SCENES_API void setSelected(std::vector<sceneObject*>* sceneObjects, bool selected);

        SCENES_API sceneObject* getSceneObject(GLuint index) const { return (*_allObjects)[index]; }
        SCENES_API sceneObject* getSceneObjectById(unsigned int id);

        SCENES_API directionalLight* getDirectionalLight(GLuint index) const { return (*_directionalLights)[index]; }
        SCENES_API pointLight* getPointLight(GLuint index) const { return (*_pointLights)[index]; }
        SCENES_API spotLight* getSpotLight(GLuint index) const { return (*_spotLights)[index]; }

        SCENES_API camera* getCamera(GLuint index) const { return (*_cameras)[index]; }

        SCENES_API void save(std::string path);
        SCENES_API void load(std::string path);

        SCENES_API void remove(sceneObject* sceneObject);
    };
}
#endif