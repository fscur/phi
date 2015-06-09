#include "panCommand.h"
#include "scenesManager.h"

namespace phi
{
    panCommand::panCommand()
    {
        _request = nullptr;
    }

    panCommand::~panCommand()
    {
        DELETE(_request);
    }

    void panCommand::init()
    {
        _request = new bufferRequest((unsigned int)info->mousePos.x, (unsigned int)info->mousePos.y, (unsigned int)1, (unsigned int)1, dynamic_cast<frameBuffer*>(renderingSystem::defaultFrameBuffer));
        renderingCommunicationBuffer::request(_request);
    }

    bool panCommand::canExecute()
    {
        if (!_request->mutex.try_lock())
            return false;

        if (!_request->completed)
        {
            _request->mutex.unlock();
            return false;
        }

        renderingCommunicationBuffer::removeRequest(_request);
        _zBufferValue = _request->result;
        _request->mutex.unlock();
        DELETE(_request);

        phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
        glm::mat4 proj = camera->getPerspProjMatrix();

        _eyeZ = -proj[3].z / (_zBufferValue * -2.0 + 1.0 - proj[2].z);
        LOG(_eyeZ);
        _cameraPos = camera->getPosition();
        _cameraRight = camera->getRight();
        _cameraUp = camera->getUp();
        _startPos = info->mousePos;
        return true;
    }

    void panCommand::update()
    {
        if (_zBufferValue == 1.0f)
            return;

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

        float ys0 = _startPos.y - hh;
        float yp0 = ys0/hh;
        float ym0 = -(yp0 * tg);

        float xs0 = _startPos.x - hw;
        float xp0 = xs0/hw;
        float xm0 = xp0 * tg * aspect;

        float ys1 = info->mousePos.y - hh;
        float yp1 = ys1/hh;
        float ym1 = -(yp1 * tg);

        float xs1 = info->mousePos.x - hw;
        float xp1 = xs1/hw;
        float xm1 = xp1 * tg * aspect;

        float eyeZ = _eyeZ;

        float xDiff = xm1 - xm0;
        float yDiff = ym1 - ym0;

        float x = xDiff * (eyeZ/zNear);
        float y = yDiff * (eyeZ/zNear);

        glm::vec3 pos = _cameraPos - (glm::vec3(_cameraRight * x) + glm::vec3(_cameraUp * y)); 

        camera->moveTo(pos);
    }
}