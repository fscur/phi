#include "scene.h"
#include "plane.h"
#include "sphere.h"
#include "renderingSystem.h"
#include <fstream>
#include <iostream>

namespace phi
{
    scene::scene()
    {
        _sceneObjectsIds = 0;
        _allObjectsCount = 0;
        _visibleObjectsCount = 0;
        _staticObjectsCount = 0;
        _dynamicObjectsCount = 0;
        _size = size<GLuint>(800, 600);
        _deltaTime = 0.008f;
        _allObjects = new std::vector<sceneObject*>();
        _directionalLights = new std::vector<directionalLight*>();
        _pointLights = new std::vector<pointLight*>();
        _spotLights = new std::vector<spotLight*>();
        _cameras = new std::vector<camera*>();
        _selectedSceneObjectChanged = new eventHandler<sceneObjectEventArgs>();
        _staticObjectsChanged = new eventHandler<eventArgs>();

        _activeCamera = new camera(0.1f, 1000.0f, 800.0f / 600.0f, phi::PI_OVER_4);
        _cameras->push_back(_activeCamera);
    }

    scene::~scene()
    {
        for (GLuint i = 0; i < _allObjects->size(); i++)
            DELETE((*_allObjects)[i]);

        for (GLuint i = 0; i < _directionalLights->size(); i++)
            DELETE((*_directionalLights)[i]);

        for (GLuint i = 0; i < _pointLights->size(); i++)
            DELETE((*_pointLights)[i]);

        for (GLuint i = 0; i < _spotLights->size(); i++)
            DELETE((*_spotLights)[i]);

        for (GLuint i = 0; i < _cameras->size(); i++)
            DELETE((*_cameras)[i]);
    }

    void scene::input()
    {
        /*
        if (Keyboard::IsPressed(Keys::N1))
        _isShadowMapEnabled = !_isShadowMapEnabled;

        if (Keyboard::IsPressed(Keys::O))
        {
        _rotationSpeed += 0.01f;
        _translationSpeed += 0.0001f;
        }
        else if (Keyboard::IsPressed(Keys::P))
        {
        _rotationSpeed -= 0.01f;
        _translationSpeed -= 0.0001f;
        }

        SceneObject* obj1 = *(_sceneObjects->begin());

        if (Keyboard::IsPressed(Keys::Z))
        obj1->Translate(glm::vec3(-_translationSpeed, 0.0f, 0.0f));
        else if (Keyboard::IsPressed(Keys::X))
        obj1->Translate(glm::vec3(_translationSpeed, 0.0f, 0.0f));
        else if (Keyboard::IsPressed(Keys::C))
        obj1->Translate(glm::vec3(0.0f, _translationSpeed, 0.0f));
        else if (Keyboard::IsPressed(Keys::V))
        obj1->Translate(glm::vec3(0.0f, -_translationSpeed, 0.0f));
        else if (Keyboard::IsPressed(Keys::B))
        obj1->Translate(glm::vec3(0.0f, 0.0f, _translationSpeed));
        else if (Keyboard::IsPressed(Keys::N))
        obj1->Translate(glm::vec3(0.0f, 0.0f, -_translationSpeed));
        else if (Keyboard::IsPressed(Keys::W))
        obj1->Pitch(_rotationSpeed * _deltaTime);
        else if (Keyboard::IsPressed(Keys::S))
        obj1->Pitch(-_rotationSpeed * _deltaTime);
        else if (Keyboard::IsPressed(Keys::A))
        obj1->Yaw(_rotationSpeed * _deltaTime);
        else if (Keyboard::IsPressed(Keys::D))
        obj1->Yaw(-_rotationSpeed * _deltaTime);*/
    }

    void scene::update()
    {
        //bool cameraChanged = _activeCamera->getChanged();

        for (GLuint i = 0; i < _cameras->size(); i++)
            (*_cameras)[i]->update();

        for (GLuint i = 0; i < _pointLights->size(); i++)
            (*_pointLights)[i]->update();

        for (GLuint i = 0; i < _spotLights->size(); i++)
            (*_spotLights)[i]->update();

        if (_allObjects->size() == 0)
            return;

        _visibleObjects.clear();
        _visibleObjectsCount = 0;

        //frustum* frustum = _cameras[0]->getFrustum();
        glm::mat4 viewMatrix = _activeCamera->getViewMatrix();
        glm::mat4 perspMatrix = _activeCamera->getPerspProjMatrix();
        glm::vec3 cameraPosition = _activeCamera->getPosition();
        sceneObject* sceneObj = nullptr;
        transform* transform = nullptr;
        glm::vec3 center;
        //float radius;

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObj = (*_allObjects)[i];
            
            auto objectId = sceneObj->getId();

            if (sceneObj->getChanged())
            {
                auto obj = _staticObjects.find(objectId);

                if (obj != _staticObjects.end())
                {
                    _staticObjects.erase(objectId);
                    _staticObjectsCount--;

                    _dynamicObjects[objectId] = sceneObj;
                    _dynamicObjectsCount++;
                
                    staticObjectsChanged(eventArgs());
                }
            }
            else
            {
                auto obj = _dynamicObjects.find(objectId);

                if (obj != _dynamicObjects.end())
                {
                    _dynamicObjects.erase(obj);
                    _dynamicObjectsCount--;

                    _staticObjects[objectId] = sceneObj;
                    _staticObjectsCount++;
                    
                    staticObjectsChanged(eventArgs());
                }
            }

            sceneObj->update();
        }
    }

    void scene::setSize(size<GLuint> size)
    {
        _size = size;
        _activeCamera->getFrustum()->setAspect((float)_size.width/(float)_size.height);
    }

    void scene::add(sceneObject* sceneObject)
    {
        sceneObject->initialize();
        _allObjects->push_back(sceneObject);
        
        _staticObjects[_sceneObjectsIds] = sceneObject;
        _staticObjectsCount++;

        sceneObject->setId(_sceneObjectsIds++);
        _allObjectsCount++;

        staticObjectsChanged(eventArgs());

        sceneObject->getIsSelectedChanged()->bind<scene, &scene::sceneObjectIsSelectedChanged>(this);
    }

    void scene::add(directionalLight* directionalLight)
    {
        _directionalLights->push_back(directionalLight);
    }

    void scene::add(pointLight* pointLight)
    {
        _pointLights->push_back(pointLight);
    }

    void scene::add(spotLight* spotLight)
    {
        _spotLights->push_back(spotLight);
    }

    void scene::add(camera* camera)
    {
        _cameras->push_back(camera);
    }

    void scene::sceneObjectIsSelectedChanged(phi::sceneObjectEventArgs e)
    {
        if (_selectedSceneObjectChanged->isBound())
            _selectedSceneObjectChanged->invoke(e);
    }

    void scene::staticObjectsChanged(phi::eventArgs e)
    {
        if (_staticObjectsChanged->isBound())
            _staticObjectsChanged->invoke(e);
    }

    sceneObject* scene::getSceneObjectById(unsigned int id)
    {
        for (unsigned int i = 0; i < _allObjectsCount; i++)
        {
            if ((*_allObjects)[i]->getId() == id)
                return (*_allObjects)[i];
        }

        return nullptr;
    }

    void scene::remove(sceneObject* sceneObj)
    {
        //std::vector<sceneObject*>::iterator position = std::find(_allObjects.begin(), _allObjects.end(), sceneObj);

        //if (position != _allObjects.end()) // == vector.end() means the element was not found
        //{
        //	_allObjects.erase(position);
        //	_allObjectsCount--;
        //}
    }

    void scene::save(std::string path)
    {
        std::ofstream stream;
        stream.open(path.c_str(), std::ios::out | std::ios::binary);

        auto objCount = (*_allObjects).size();
        for (unsigned int i = 0; i < objCount; i ++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];
            auto id = sceneObj->getId();
            stream.write((char*)&id, sizeof(unsigned int));
            auto pos = sceneObj->getPosition();
            stream.write((char*)&pos.x, sizeof(float));
            stream.write((char*)&pos.y, sizeof(float));
            stream.write((char*)&pos.z, sizeof(float));
            auto size = sceneObj->getSize();
            stream.write((char*)&size.width, sizeof(float));
            stream.write((char*)&size.height, sizeof(float));
            stream.write((char*)&size.depth, sizeof(float));
            auto quat = sceneObj->getOrientation();
            stream.write((char*)&quat.x, sizeof(float));
            stream.write((char*)&quat.y, sizeof(float));
            stream.write((char*)&quat.z, sizeof(float));
            stream.write((char*)&quat.w, sizeof(float));
        }

        char null = '\0';
        stream.write((char*)&null, 1);
        stream.close();
    }

    void scene::load(std::string path)
    {
        std::ifstream stream;
        stream.open(path.c_str(), std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);
        auto totalBytes = stream.tellg();
        stream.seekg(0, std::ios::beg);

        auto bytesPerObj = 10 * sizeof(float) + sizeof(unsigned int);
        for (auto i = 0; i < totalBytes / bytesPerObj; i++)
        {
            unsigned int id;
            stream.read((char*)&id, sizeof(unsigned int));
            float px, py, pz;
            stream.read((char*)&px, sizeof(float));
            stream.read((char*)&py, sizeof(float));
            stream.read((char*)&pz, sizeof(float));
            float w, h, d;
            stream.read((char*)&w, sizeof(float));
            stream.read((char*)&h, sizeof(float));
            stream.read((char*)&d, sizeof(float));
            float qx, qy, qz, qw;
            stream.read((char*)&qx, sizeof(float));
            stream.read((char*)&qy, sizeof(float));
            stream.read((char*)&qz, sizeof(float));
            stream.read((char*)&qw, sizeof(float));
            auto obj = getSceneObjectById(id);
            obj->setPosition(glm::vec3(px, py, pz));
            obj->setSize(size<float>(w, h, d));
            obj->setOrientation(glm::quat(qw, qx, qy, qz));
        }

        stream.close();
    }
}