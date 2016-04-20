#include <precompiled.h>
#include "defaultCameraController.h"

#include <core\time.h>

#include <loader\importer.h>

#include <rendering\camera.h>
#include <rendering\mesh.h>
#include <rendering\ray.h>

#include <apps\application.h>

namespace demon
{
    defaultCameraController::defaultCameraController(phi::scene* scene) :
        cameraController(scene->getCamera()),
        _scene(scene),
        _mousePosX(0),
        _mousePosY(0),
        _lastMousePosX(0),
        _lastMousePosY(0),
        _zoomDir(phi::vec3()),
        _zoomCameraPos(phi::vec3()),
        _zoomSpeed(0.0f),
        _zoomDistanceTraveled(0.0f),
        _zoomDistanceLimit(0.0f),
        _zoomSpeedAccumulationTime(0.0),
        _zoomInertiaTime(0.0),
        _zoomBounceAnimation(nullptr),
        _rotating(false),
        _rotationDoingInertia(false),
        _rotationDelta(phi::vec2()),
        _rotationTargetPos(phi::vec3()),
        _rotationLastMouseMoveTime(0.0),
        _rotationInertiaTime(0.0),
        _rotationInertiaLastPercent(0.0f),
        _panning(false),
        _panDoingInertia(false),
        _panEyeZ(0.0f),
        _panCameraPos(phi::vec3()),
        _panCameraRight(phi::vec3()),
        _panCameraUp(phi::vec3()),
        _panDelta(phi::vec3()),
        _panTargetCameraPos(phi::vec3()),
        _panLastMouseMoveTime(0.0),
        _panInertiaTime(0.0),
        _dragging(false),
        _dragPlaneBottomLeft(phi::vec3()),
        _dragPlaneBottomRight(phi::vec3()),
        _dragPlaneTopRight(phi::vec3()),
        _dragPlaneTopLeft(phi::vec3())
    {
        auto texturePath = phi::application::resourcesPath + "\\images\\grid.png";
        _gridTexture = phi::importer::importImage(texturePath);
        _scene->getRenderer()->getPlaneGridRenderPass()->setTexture(_gridTexture);
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
            dragMouseDown(e->x, e->y);

        if (e->middleButtonPressed && !_rotating)
            panMouseDown(e->x, e->y);

        if (e->rightButtonPressed && !_panning)
            rotationMouseDown(e->x, e->y);
    }

    void defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
    {
        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
        _mousePosX = e->x;
        _mousePosY = e->y;

        if (_dragging)
            dragMouseMove();
        if (_panning)
            panMouseMove();
        if (_rotating)
            rotationMouseMove();
    }

    void defaultCameraController::onMouseUp(phi::mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
            dragMouseUp();

        if (e->middleButtonPressed)
            panMouseUp();

        if (e->rightButtonPressed)
            rotationMouseUp();
    }

    void defaultCameraController::onMouseWheel(phi::mouseEventArgs* e)
    {
        if (!_panning && !_rotating)
            zoomMouseWheel(e->x, e->y, e->wheelDelta);
    }

    void defaultCameraController::update()
    {
        zoomUpdate();
        rotationUpdate();
        panUpdate();

        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
    }

    void defaultCameraController::dragMouseDown(int mouseX, int mouseY)
    {
        float w = (float)_camera->getWidth();
        float h = (float)_camera->getHeight();
        float x = (2.0f * mouseX) / w - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / h;

        auto invPersp = inverse(_camera->getProjectionMatrix());
        auto invView = inverse(_camera->getViewMatrix());

        auto rayClip = phi::vec4(x, y, -1.0f, 1.0f);
        auto rayEye = invPersp * rayClip;
        rayEye = phi::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        auto rayWorld = phi::vec3(invView * rayEye);
        rayWorld = glm::normalize(rayWorld);

        auto r = phi::ray(_camera->getTransform()->getLocalPosition(), rayWorld);

        auto object = _scene->getObjects()->at(1);
        phi::aabb* aabb = new phi::aabb();
        object->traverse
        (
            [&aabb](phi::node* n)
            {
                auto meshComponent = n->getComponent<phi::mesh>();
                if (meshComponent)
                {
                    auto added = new phi::aabb(phi::aabb::add(*meshComponent->geometry->aabb, *aabb));
                    safeDelete(aabb);
                    aabb = added;
                }
            }
        );

        auto model = object->getTransform()->getModelMatrix();
        auto transformedMin = phi::mathUtils::multiply(model, aabb->min);
        auto transformedMax = phi::mathUtils::multiply(model, aabb->max);
        auto transformedAabb = phi::aabb(transformedMin, transformedMax);

        phi::vec3* positions;
        phi::vec3* normals;
        size_t count;
        if (r.intersects(transformedAabb, positions, normals, count))
        {
            auto normal = normals[0];

            _dragOrigin = positions[0];
            _dragObject = object;
            _dragObjectStartPosition = object->getTransform()->getLocalPosition();

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);

            auto min = transformedAabb.min;
            auto max = transformedAabb.max;
            auto lbb = phi::vec3(min.x, min.y, min.z);
            auto lbf = phi::vec3(min.x, min.y, max.z);
            auto ltf = phi::vec3(min.x, max.y, max.z);
            auto ltb = phi::vec3(min.x, max.y, min.z);
            auto rbb = phi::vec3(max.x, min.y, min.z);
            auto rbf = phi::vec3(max.x, min.y, max.z);
            auto rtf = phi::vec3(max.x, max.y, max.z);
            auto rtb = phi::vec3(max.x, max.y, min.z);

            if (normal == phi::vec3(-1.0f, 0.0f, 0.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(rbf);
                _dragPlaneBottomLeft = lbb;
                _dragPlaneBottomRight = lbf;
                _dragPlaneTopRight = ltf;
                _dragPlaneTopLeft = ltb;
            }
            else if (normal == phi::vec3(1.0f, 0.0f, 0.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(lbb);
                _dragPlaneBottomLeft = rbf;
                _dragPlaneBottomRight = rbb;
                _dragPlaneTopRight = rtb;
                _dragPlaneTopLeft = rtf;
            }
            else if (normal == phi::vec3(0.0f, 0.0f, 1.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(rbb);
                _dragPlaneBottomLeft = lbf;
                _dragPlaneBottomRight = rbf;
                _dragPlaneTopRight = rtf;
                _dragPlaneTopLeft = ltf;
            }
            else if (normal == phi::vec3(0.0f, 0.0f, -1.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(lbf);
                _dragPlaneBottomLeft = rbb;
                _dragPlaneBottomRight = lbb;
                _dragPlaneTopRight = ltb;
                _dragPlaneTopLeft = rtb;
            }
            else if (normal == phi::vec3(0.0f, 1.0f, 0.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(lbb);
                _dragPlaneBottomLeft = ltf;
                _dragPlaneBottomRight = rtf;
                _dragPlaneTopRight = rtb;
                _dragPlaneTopLeft = ltb;
            }
            else if (normal == phi::vec3(0.0f, -1.0f, 0.0f))
            {
                _scene->getRenderer()->getPlaneGridRenderPass()->transform.setLocalPosition(ltf);
                _dragPlaneBottomLeft = lbb;
                _dragPlaneBottomRight = rbb;
                _dragPlaneTopRight = rbf;
                _dragPlaneTopLeft = lbf;
            }

            _scene->getRenderer()->getPlaneGridRenderPass()->transform.setDirection(normal);

            _dragging = true;
            _scene->getRenderer()->getPlaneGridRenderPass()->show();
        }

        safeDelete(aabb);
    }

    void defaultCameraController::dragMouseMove()
    {
        auto w = (float)_camera->getWidth();
        auto h = (float)_camera->getHeight();
        auto x = (2.0f * _mousePosX) / w - 1.0f;
        auto y = 1.0f - (2.0f * _mousePosY) / h;

        auto invPersp = inverse(_camera->getProjectionMatrix());
        auto invView = inverse(_camera->getViewMatrix());

        auto rayClip = phi::vec4(x, y, -1.0f, 1.0f);
        auto rayEye = invPersp * rayClip;
        rayEye = phi::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        auto rayWorld = phi::vec3(invView * rayEye);
        rayWorld = glm::normalize(rayWorld);

        auto r = phi::ray(_camera->getTransform()->getLocalPosition(), rayWorld);

        auto bl = _dragPlaneBottomLeft;
        auto br = _dragPlaneBottomRight;
        auto tr = _dragPlaneTopRight;
        auto tl = _dragPlaneTopLeft;
        auto planeNormal = normalize(cross(bl - br, br - tr));
        auto d = dot(planeNormal, bl);
        auto t = (d - dot(planeNormal, r.getOrigin())) / (dot(planeNormal, (r.getDirection())));
        auto nDotA = dot(planeNormal, r.getOrigin());
        auto nDotBA = dot(planeNormal, r.getDirection());
        auto point = r.getOrigin() + (((d - nDotA) / nDotBA) * r.getDirection());

        auto diff = point - _dragOrigin;

        _dragObject->getTransform()->setLocalPosition(_dragObjectStartPosition + diff);
    }

    void defaultCameraController::dragMouseUp()
    {
        if (_dragging)
        {
            _scene->getRenderer()->getPlaneGridRenderPass()->hide();
            _dragging = false;
        }
    }

    void defaultCameraController::zoomMouseWheel(int mouseX, int mouseY, float delta)
    {
        if (delta > 0.0f && _zoomBounceAnimation)
            return;

        if (_zoomBounceAnimation)
        {
            phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);
            _zoomBounceAnimation = nullptr;
        }

        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);

        phi::mat4 proj = _camera->getProjectionMatrix();

        float z;
        if (zBufferValue == 1.0f)
            z = 10.0f;
        else
            z = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto zNear = _camera->getNear();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getWidth();
        auto h = _camera->getHeight();

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = mouseY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mouseX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto x = (xm0 / zNear) * z;
        auto y = (ym0 / zNear) * z;

        auto cameraTransform = _camera->getTransform();
        phi::vec3 camDir = cameraTransform->getDirection();
        phi::vec3 camRight = cameraTransform->getRight();
        phi::vec3 camUp = cameraTransform->getUp();

        _zoomDir = glm::normalize(camDir * z + -camRight * static_cast<float>(x) + camUp * static_cast<float>(y));

        _zoomInertiaTime = 0.0;
        _zoomDistanceTraveled = 0.0f;

        if (glm::sign(_zoomSpeed) != glm::sign(delta))
            _zoomSpeed = 0.0f;

        if (_zoomSpeedAccumulationTime <= 0.0)
        {
            _zoomSpeed = 0.0f;
            _zoomSpeedAccumulationTime = ZOOM_ACCUMULATION_TIME;
        }

        _zoomSpeed += (delta / 120.0f) * ZOOM_FACTOR;
        _zoomDistanceLimit = glm::max(z - zNear - ZOOM_MAX_BOUNCE - 0.0001f, 0.0f); // Small gap so the camera does not go past the z-near plane
        _zoomCameraPos = cameraTransform->getPosition();
    }

    void defaultCameraController::zoomUpdate()
    {
        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _zoomSpeedAccumulationTime = glm::max(_zoomSpeedAccumulationTime - deltaMilliseconds, 0.0);

        if (_zoomSpeed == 0.0f)
            return;

        _zoomInertiaTime += deltaMilliseconds;
        auto percent = static_cast<float>(-glm::exp(_zoomInertiaTime / -325.0f) + 1.0f);
        auto delta = _zoomSpeed * percent;

        if (percent > 0.998f)
            _zoomSpeed = 0.0f;

        if (delta > _zoomDistanceLimit)
        {
            _camera->getTransform()->setLocalPosition(_zoomCameraPos + _zoomDir * _zoomDistanceLimit);
            _zoomDistanceTraveled = _zoomDistanceLimit;
            _zoomSpeed = 0.0f;

            zoomCancel();

            auto speedPercent = glm::clamp(delta / 2.0f, 0.0f, 1.0f);
            auto bounceDistance = ZOOM_MIN_BOUNCE + (ZOOM_MAX_BOUNCE * speedPercent);
            auto cameraPosition = _camera->getTransform()->getPosition();

            _zoomBounceAnimation = new phi::floatAnimation
            (
                0.0f,
                bounceDistance,
                400,
                [this, cameraPosition](float t)
            {
                _camera->getTransform()->setLocalPosition(cameraPosition + _zoomDir * t);
            },
                0,
                phi::easingFunctions::easeRubberBack,
                [&] { _zoomBounceAnimation = nullptr; }
            );

            phi::floatAnimator::animateFloat(_zoomBounceAnimation);
        }
        else
            _camera->getTransform()->setLocalPosition(_zoomCameraPos + _zoomDir * delta);
    }

    void defaultCameraController::zoomCancel()
    {
        if (_zoomBounceAnimation)
        {
            phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);
            _zoomBounceAnimation = nullptr;
        }

        _zoomSpeed = 0.0f;
    }

    void defaultCameraController::panMouseDown(int mouseX, int mouseY)
    {
        zoomCancel();
        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);
        phi::mat4 proj = _camera->getProjectionMatrix();

        if (zBufferValue == 1.0f)
            _panEyeZ = 20.0f;
        else
            _panEyeZ = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto cameraTransform = _camera->getTransform();
        _panCameraPos = _panTargetCameraPos = cameraTransform->getPosition();
        _panCameraRight = cameraTransform->getRight();
        _panCameraUp = cameraTransform->getUp();
        _panDelta = glm::vec3();
        _panning = true;
    }

    void defaultCameraController::panMouseMove()
    {
        auto zNear = _camera->getNear();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getWidth();
        auto h = _camera->getHeight();

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = (float)_lastMousePosY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = (float)_lastMousePosX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto ys1 = (float)_mousePosY - hh;
        auto yp1 = ys1 / hh;
        auto ym1 = -(yp1 * tg);

        auto xs1 = (float)_mousePosX - hw;
        auto xp1 = xs1 / hw;
        auto xm1 = xp1 * tg * aspect;

        auto xDiff = xm1 - xm0;
        auto yDiff = ym1 - ym0;

        float x = xDiff * (_panEyeZ / zNear);
        float y = yDiff * (_panEyeZ / zNear);

        auto delta = phi::vec2(x, y);
        auto deltaWorld = phi::vec3(_panCameraRight * delta.x) + phi::vec3(-_panCameraUp * delta.y);

        _panCameraPos = _camera->getTransform()->getPosition();
        _panTargetCameraPos += deltaWorld;
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panInertiaTime = 0.0;
        _panLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _panDoingInertia = true;
    }

    void defaultCameraController::panMouseUp()
    {
        _panning = false;
        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _panLastMouseMoveTime));
        _panCameraPos = _camera->getTransform()->getPosition();
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panDelta += _panDelta * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f)));
    }

    void defaultCameraController::panUpdate()
    {
        if (!_panDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _panInertiaTime += deltaMilliseconds;
        auto desFactor = _panning ? 100.0f : 325.0f;
        auto percent = -glm::exp(_panInertiaTime / -desFactor) + 1.0f;
        auto delta = static_cast<float>(glm::length(_panDelta) * percent);

        if (delta == 0.0f)
            return;

        auto dir = glm::normalize(_panDelta);
        _camera->getTransform()->setLocalPosition(_panCameraPos + dir * delta);

        if (percent >= 1.0f)
            _panDoingInertia = false;
    }

    void defaultCameraController::panCancel()
    {
        _panDelta = phi::vec3();
    }

    void defaultCameraController::rotationMouseDown(int mouseX, int mouseY)
    {
        zoomCancel();
        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);
        auto proj = _camera->getProjectionMatrix();

        if (zBufferValue == 1.0f)
            _rotationTargetPos = glm::vec3(); //_targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
        else
        {
            auto z = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);
            auto zNear = _camera->getNear();
            auto aspect = _camera->getAspect();
            auto fov = _camera->getFov();

            auto tg = tan(fov * 0.5f) * zNear;

            auto w = _camera->getWidth();
            auto h = _camera->getHeight();

            auto hh = h * 0.5f;
            auto hw = w * 0.5f;

            auto ys0 = mouseY - hh;
            auto yp0 = ys0 / hh;
            auto ym0 = -(yp0 * tg);

            auto xs0 = mouseX - hw;
            auto xp0 = xs0 / hw;
            auto xm0 = xp0 * tg * aspect;

            auto x = (xm0 / zNear) * z;
            auto y = (ym0 / zNear) * z;

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _rotationTargetPos = camPos + camDir * z + -camRight * (float)x + camUp * (float)y;
        }

        _rotationLastMouseMoveTime = 0.0;
        _rotating = true;
    }

    void defaultCameraController::rotationMouseMove()
    {
        auto w = _camera->getWidth();
        auto h = _camera->getHeight();

        auto dx = _mousePosX - _lastMousePosX;
        auto dy = _mousePosY - _lastMousePosY;

        auto x = (dx / w) * 3.0f * phi::PI;
        auto y = (dy / h) * 3.0f * phi::PI;

        auto delta = phi::vec2(x, y);

        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(delta) * (lastRemainingRotation + glm::length(delta));

        _rotationLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _rotationInertiaTime = 0.0;
        _rotationDoingInertia = true;
        _rotationInertiaLastPercent = 0.0f;
    }

    void defaultCameraController::rotationMouseUp()
    {
        _rotating = false;

        if (glm::length(_rotationDelta) == 0.0f)
            return;

        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _rotationLastMouseMoveTime));
        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(_rotationDelta) * (lastRemainingRotation + lastRemainingRotation * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f))));
    }

    void defaultCameraController::rotationUpdate()
    {
        if (!_rotationDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0f;
        _rotationInertiaTime += deltaMilliseconds;
        auto desFactor = _rotating ? 100.0f : 325.0f;
        auto percent = static_cast<float>(-glm::exp(_rotationInertiaTime / -desFactor) + 1.0);
        auto delta = _rotationDelta * (percent - _rotationInertiaLastPercent);
        _rotationInertiaLastPercent = percent;

        _camera->orbit(_rotationTargetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getTransform()->getRight(), -delta.x, -delta.y);
        if (percent >= 1.0f)
            _rotationDoingInertia = false;
    }

    void defaultCameraController::rotationCancel()
    {
        _rotationDelta = phi::vec2();
    }
}