#ifndef _PHI_SCENE_H_
#define _PHI_SCENE_H_

#include "rendering.h"
#include <typeinfo>

#include "globals.h"
#include "mathUtils.h"
#include "resourceManager.h"
#include "shaderManager.h"

#include "sceneObject.h"
#include "camera.h"
#include "material.h"

#include "defaultRenderTarget.h"
#include "shadowMapRenderTarget.h"
#include "gBuffer.h"

#include "meshRenderer.h"

#include "directionalLight.h"
#include "pointLight.h"
#include "spotLight.h"

namespace phi
{
    class scene
    {
    private:
        GLuint _allObjectsCount;
        GLuint _visibleObjectsCount;
        size<GLuint> _size;
        float _deltaTime;
		color _backColor;
		color _ambientColor;

        camera *_activeCamera;

        std::vector<sceneObject*>* _allObjects;
        std::vector<sceneObject*> _visibleObjects;

		std::vector<directionalLight*>* _directionalLights;
		std::vector<pointLight*>* _pointLights;
		std::vector<spotLight*>* _spotLights;

        std::vector<camera*> _cameras;


    private:
        void debugRender();

    public:
        RENDERING_API scene();
        RENDERING_API ~scene();

        RENDERING_API camera* getActiveCamera() const { return _activeCamera; }
		RENDERING_API color getBackColor() const { return _backColor; }
		RENDERING_API color getAmbientColor() const { return _ambientColor; }
        RENDERING_API std::vector<sceneObject*>* getAllObjects() { return _allObjects; }
        RENDERING_API std::vector<sceneObject*> getVisibleObjects() const { return _visibleObjects; }
        RENDERING_API GLuint getVisibleObjectsCount() const { return _visibleObjectsCount; }
        RENDERING_API GLuint getAllObjectsCount() const { return _allObjectsCount; }
		RENDERING_API std::vector<directionalLight*>* getDirectionalLights() const { return _directionalLights; }
		RENDERING_API std::vector<pointLight*>* getPointLights() const { return _pointLights; }
		RENDERING_API std::vector<spotLight*>* getSpotLights() const { return _spotLights; }

        RENDERING_API void setSize(size<GLuint> size);
		RENDERING_API void setActiveCamera(camera* value) { _activeCamera = value; }
		RENDERING_API void setBackColor(color value) { _backColor = value; }
		RENDERING_API void setAmbientLightColor(color value) { _ambientColor = value; }
        RENDERING_API void setDeltaTime(float value) { _deltaTime = value; }

        RENDERING_API void input();
        RENDERING_API void update();
        RENDERING_API void render();

        RENDERING_API void add(sceneObject* sceneObject);
		RENDERING_API void add(directionalLight* directionalLight);
		RENDERING_API void add(pointLight* pointLight);
		RENDERING_API void add(spotLight* spotLight);
        RENDERING_API void add(camera* camera);

		RENDERING_API sceneObject* getSceneObject(GLuint index) const { return (*_allObjects)[index]; }
		RENDERING_API directionalLight* getDirectionalLight(GLuint index) const { return (*_directionalLights)[index]; }
		RENDERING_API pointLight* getPointLight(GLuint index) const { return (*_pointLights)[index]; }
		RENDERING_API spotLight* getSpotLight(GLuint index) const { return (*_spotLights)[index]; }

        RENDERING_API void remove(sceneObject* sceneObject);
    };
}
#endif