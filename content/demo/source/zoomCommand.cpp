#include "zoomCommand.h"
#include "scenesManager.h"
#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "box.h"

namespace phi
{
    zoomCommand::zoomCommand(bool in)
    {
        _in = in;
        _request = nullptr;
    }

    zoomCommand::~zoomCommand()
    {
        DELETE(_request);
    }

    void zoomCommand::init()
    {
        _request = new bufferRequest((unsigned int)info->mousePos.x, (unsigned int)info->mousePos.y, (unsigned int)1, (unsigned int)1, dynamic_cast<frameBuffer*>(renderingSystem::defaultFrameBuffer));
        renderingCommunicationBuffer::request(_request);
    }

    bool zoomCommand::canExecute()
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
        _request = nullptr;

        return true;
    }

    void zoomCommand::update()
    {
        if (_zBufferValue == 1.0f)
            return;

        phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
        glm::mat4 proj = camera->getPerspProjMatrix();

        glm::vec2 mousePos = info->mousePos;
        float z = -proj[3].z / (_zBufferValue * -2.0 + 1.0 - proj[2].z);

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

        if (_in)
            camera->zoomIn(targetPos);
        else
            camera->zoomOut(targetPos);

        return;
    }
}