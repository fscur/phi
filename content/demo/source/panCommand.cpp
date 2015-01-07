#include "panCommand.h"
#include "scenesManager.h"

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

namespace phi
{
	commandInfo* panStartCommand::execute(commandInfo* info)
	{
		panStartCommandInfo* panStartInfo = new panStartCommandInfo(*info);

		GLfloat zBufferValue;
	
		phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
		glm::mat4 proj = camera->getPerspProjMatrix();
	
		glm::vec2 mousePos = info->mousePos;	

		glReadPixels(mousePos.x, info->viewportSize.height - mousePos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

		panStartInfo->eyeZ = -proj[3].z / (zBufferValue * -2.0 + 1.0 - proj[2].z);
		panStartInfo->cameraPos = camera->getPosition();
		panStartInfo->cameraRight = camera->getRight();
		panStartInfo->cameraUp = camera->getUp();
		panStartInfo->startPos = mousePos;

		DELETE(info);

		LOG("PAN START COMMAND");

		return panStartInfo;
	}

	commandInfo* panCommand::execute(commandInfo* info)
	{
		panStartCommandInfo* panStartInfo = dynamic_cast<panStartCommandInfo*>(info);
		
		if (panStartInfo == nullptr)
			return nullptr;

		phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
		phi::frustum* frustum = camera->getFrustum();
		
		float zNear = frustum->getZNear();
		float iez = 1.0f / zNear;
		float zFar = frustum->getZFar();
		float aspect = frustum->getAspect();
		float fov = frustum->getFov();

		float tg = glm::tan(fov * 0.5f) * zNear;

		float h = info->viewportSize.height;
		float w = info->viewportSize.width;

		float hh = h * 0.5f;
		float hw = w * 0.5f;

		float ys0 = panStartInfo->startPos.y - hh;
		float yp0 = ys0/hh;
		float ym0 = -(yp0 * tg);

		float xs0 = panStartInfo->startPos.x - hw;
		float xp0 = xs0/hw;
		float xm0 = xp0 * tg * aspect;

		float ys1 = info->mousePos.y - hh;
		float yp1 = ys1/hh;
		float ym1 = -(yp1 * tg);

		float xs1 = info->mousePos.x - hw;
		float xp1 = xs1/hw;
		float xm1 = xp1 * tg * aspect;

		float eyeZ = panStartInfo->eyeZ;

		float xDiff = xm1 - xm0;
		float yDiff = ym1 - ym0;

		float x = xDiff * eyeZ/zNear;
		float y = yDiff * eyeZ/zNear;

		glm::vec3 pos = panStartInfo->cameraPos - (glm::vec3(panStartInfo->cameraRight * x) + glm::vec3(panStartInfo->cameraUp * y)); 

		camera->setPosition(pos);

		LOG("PAN COMMAND");

		return panStartInfo;
	}
}