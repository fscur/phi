#include "rotateCommand.h"
#include "scenesManager.h"


#include "box.h"
namespace phi
{
	commandInfo* rotateStartCommand::execute(commandInfo* info)
	{
		GLfloat zBufferValue = renderingSystem::defaultFrameBuffer->getZBufferValue(info->mousePos);
		
		if (zBufferValue == 1.0f)
			return nullptr;
		
		LOG("r: " << std::to_string(zBufferValue));

		rotateCommandInfo* rotateInfo = new rotateCommandInfo(*info);
		phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
		glm::mat4 proj = camera->getPerspProjMatrix();
	
		glm::vec2 mousePos = info->mousePos;	
		rotateInfo->lastMousePos = mousePos;

		float z = -proj[3].z / (zBufferValue * -2.0 + 1.0 - proj[2].z);

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

		float ys0 = mousePos.y - hh;
		float yp0 = ys0/hh;
		float ym0 = -(yp0 * tg);

		float xs0 = mousePos.x - hw;
		float xp0 = xs0/hw;
		float xm0 = xp0 * tg * aspect;

		float x = (xm0/zNear) * (z);
		float y = (ym0/zNear) * (z);

		glm::vec3 camPos = camera->getPosition();
		glm::vec3 camDir = camera->getDirection();
		glm::vec3 camRight = camera->getRight();
		glm::vec3 camUp = camera->getUp();

		glm::vec3 targetPos = camPos + camDir * z + camRight * x + camUp * y;

		rotateInfo->targetPos = targetPos;
		rotateInfo->zEye = z;

		return rotateInfo;
	}

	commandInfo* rotateCommand::execute(commandInfo* info)
	{
		rotateCommandInfo* rotateInfo = dynamic_cast<rotateCommandInfo*>(info);

		if (rotateInfo == nullptr)
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

		float dx = rotateInfo->lastMousePos.x - rotateInfo->mousePos.x;
		float dy = rotateInfo->lastMousePos.y - rotateInfo->mousePos.y;
		
		float x = (dx/w) * 3 * PI;
		float y = (dy/h) * 3 * PI;
		
		camera->orbit(rotateInfo->targetPos, glm::vec3(0.0, 1.0, 0.0), x);
		camera->orbit(rotateInfo->targetPos, camera->getRight(), y);
	
		return rotateInfo;
	}
	
	commandInfo* rotateEndCommand::execute(commandInfo* info)
	{
		DELETE(info);

		return nullptr;
	}
}