#include "scene.h"
#include "plane.h"
#include "sphere.h"

namespace phi
{
	scene::scene()
	{
		_allObjectsCount = 0;
		_visibleObjectsCount = 0;
		_size = size<GLuint>(800, 600);
		_deltaTime = 0.008f;
		_activeCamera = new camera(0.1f, 1000.0f, 800.0f / 600.0f, 0.78539816f);
		_cameras.push_back(_activeCamera);
		_allObjects = new std::vector<sceneObject*>();
		_directionalLights = new std::vector<directionalLight*>();
		_pointLights = new std::vector<pointLight*>();
		_spotLights = new std::vector<spotLight*>();
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

		for (GLuint i = 0; i < _cameras.size(); i++)
			DELETE(_cameras[i]);
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
		//bool cameraChanged = _cameras[1]->GetChanged();

		for (GLuint i = 0; i < _cameras.size(); i++)
			_cameras[i]->update();

		for (GLuint i = 0; i < _pointLights->size(); i++)
			(*_pointLights)[i]->update();

		for (GLuint i = 0; i < _spotLights->size(); i++)
			(*_spotLights)[i]->update();

		if (_allObjects->size() == 0)
			return;

		//if (cameraChanged)
		//{
		_visibleObjects.clear();
		_visibleObjectsCount = 0;

		frustum* frustum = _cameras[0]->getFrustum();
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
			transform = sceneObj->getTransform();
			transform->setViewMatrix(viewMatrix);
			transform->setProjectionMatrix(perspMatrix);

			sceneObj->update();

			/*center = sceneObj->getAabb()->getCenter();
			radius = sceneObj->getAabb()->getRadius();

			if (frustum->isInside(center, radius))
			{
			_visibleObjects.push_back(sceneObj);
			_visibleObjectsCount++;
			}*/
		}
		/*}
		else
		{
		SceneObject** sceneObjects = &_allObjects[0];

		for (unsigned int i = 0; i < _allObjectsCount; i++)
		sceneObjects[i]->Update();
		}*/
	}

	void scene::debugRender()
	{
//		_defaultRenderTarget->bind();
//		_defaultRenderTarget->clear();
//
//#ifdef WIN32
//		glm::mat4 proj = _activeCamera->getPerspProjMatrix();
//
//		glMatrixMode(GL_PROJECTION);
//		glLoadMatrixf((const GLfloat*)&proj[0]);
//
//		glMatrixMode(GL_MODELVIEW);
//		glm::mat4 MV = _activeCamera->getViewMatrix();
//		glLoadMatrixf((const GLfloat*)&MV[0]);
//
//		sceneObject** sceneObjects = &_allObjects[0];
//
//		for (GLuint i = 0; i < _allObjectsCount; i++)
//			//if (_cameras[1]->GetFrustum().IsInside(_visibleObjects[i]))
//				sceneObjects[i]->debugRender();
//
//		/*unsigned int count = _allObjects.size();
//
//		frustum frustum = _cameras[1]->GetFrustum();
//
//		for (unsigned int i = 0; i < count; i++)
//		{
//		SceneObject* sceneObject = _allObjects[i];
//		if (frustum.IsInside(sceneObject))
//		sceneObject->DebugRender();
//		}*/
//
//		/*unsigned int count = _cameras.size();
//
//		for (unsigned int i = 0; i < count; i++)
//		if (_activeCamera != _cameras[i])
//		_cameras[i]->DebugRender();*/
//#endif
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
		_allObjectsCount++;
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
		_cameras.push_back(camera);
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

}