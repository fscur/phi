#include "panCommand.h"
#include "scenesManager.h"

namespace phi
{
	commandInfo* panStartCommand::execute(commandInfo* info)
	{
		GLfloat zBufferValue = renderingSystem::defaultFrameBuffer->getZBufferValue(info->mousePos);
		
		if (zBufferValue == 1.0f)
			return nullptr;
		
		panCommandInfo* panInfo = new panCommandInfo(*info);
	
		phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
		glm::mat4 proj = camera->getPerspProjMatrix();
	
		panInfo->eyeZ = -proj[3].z / (zBufferValue * -2.0 + 1.0 - proj[2].z);
		panInfo->cameraPos = camera->getPosition();
		panInfo->cameraRight = camera->getRight();
		panInfo->cameraUp = camera->getUp();
		panInfo->startPos = info->mousePos;

		return panInfo;
	}

	commandInfo* panCommand::execute(commandInfo* info)
	{
		panCommandInfo* panInfo = dynamic_cast<panCommandInfo*>(info);

		if (panInfo == nullptr)
			return info;

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

		float ys0 = panInfo->startPos.y - hh;
		float yp0 = ys0/hh;
		float ym0 = -(yp0 * tg);

		float xs0 = panInfo->startPos.x - hw;
		float xp0 = xs0/hw;
		float xm0 = xp0 * tg * aspect;

		float ys1 = info->mousePos.y - hh;
		float yp1 = ys1/hh;
		float ym1 = -(yp1 * tg);

		float xs1 = info->mousePos.x - hw;
		float xp1 = xs1/hw;
		float xm1 = xp1 * tg * aspect;

		float eyeZ = panInfo->eyeZ;

		float xDiff = xm1 - xm0;
		float yDiff = ym1 - ym0;

		float x = xDiff * eyeZ/zNear;
		float y = yDiff * eyeZ/zNear;

		glm::vec3 pos = panInfo->cameraPos - (glm::vec3(panInfo->cameraRight * x) + glm::vec3(panInfo->cameraUp * y)); 

		camera->moveTo(pos);

		return panInfo;
	}

	commandInfo* panEndCommand::execute(commandInfo* info)
	{
		DELETE(info);

		return nullptr;
	}
}