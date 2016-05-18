#include <precompiled.h>
#include "defaultCameraController.h"

#include <core\time.h>

#include <loader\importer.h>

#include <rendering\camera.h>
#include <core\mesh.h>
#include <core\ray.h>

#include <apps\application.h>

using namespace phi;

namespace demon
{
    defaultCameraController::defaultCameraController(phi::scene* scene) :
        cameraController(scene->getCamera()),
        _scene(scene),
        _selectionMouseController(new selectionMouseController(scene)),
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
        _dragPlaneTopLeft(phi::vec3()),
        _dragDoingInertia(false),
        _dragInertiaTime(0.0),
        _dragStartPos(phi::vec2()),
        _dragDelta(phi::vec2())
    {
        auto texturePath = phi::application::resourcesPath + "\\images\\grid.png";
        _gridImage = phi::importer::importImage(texturePath);
        _planeGridPass = _scene->getRenderer()->getPlaneGridRenderPass();
        _planeGridPass->setImage(_gridImage);

        //auto object = _scene->getObjects()->at(1);
        //moveObject(object, vec3(10.0f * 10.40250898400000068022563937120139598846435546875, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
    }

    defaultCameraController::~defaultCameraController()
    {
        phi::safeDelete(_selectionMouseController);
        phi::safeDelete(_gridImage);
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        _dragObject = nullptr;

        if (e->leftButtonPressed)
        {
            _selectionMouseController->onMouseDown(e->x, e->y);

            if (_selectionMouseController->hasSelectedObjects())
                _dragObject = _selectionMouseController->getSelectedObjects()->at(0);

            dragMouseDown(e->x, e->y);
        }

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
        if (!_dragging)
        {
            auto hehe = [](double v) -> string
            {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(60) << v;
                return stream.str();
            };
            //phi::debug(hehe(time::deltaSeconds));
            auto object = _scene->getObjects()->at(1);
            //moveObject(object, vec3(10.0f * time::deltaSeconds, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
        }

        dragUpdate();
        zoomUpdate();
        rotationUpdate();
        panUpdate();

        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
    }

    phi::vec2 defaultCameraController::planePointProjection2D(phi::vec3 planeOrigin, phi::vec3 planeNormal, phi::vec3 planeRight, phi::vec3 planeUp, phi::vec3 point)
    {
        auto x = dot(planeRight, point - planeOrigin);
        auto y = dot(planeUp, point - planeOrigin);

        return phi::vec2(x, y);
    }

    phi::aabb* defaultCameraController::getTransformedAabb(phi::node* node, phi::transform* transform)
    {
        phi::aabb* aabb = new phi::aabb();
        node->traverse
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

        auto model = transform->getModelMatrix();
        auto transformedMin = phi::mathUtils::multiply(model, aabb->min);
        auto transformedMax = phi::mathUtils::multiply(model, aabb->max);
        safeDelete(aabb);
        return new phi::aabb(transformedMin, transformedMax);
    }

    phi::obb defaultCameraController::getTransformedObb(boxCollider* collider, phi::transform* transform)
    {
        auto position = collider->getPosition() + transform->getPosition();
        return obb(position, transform->getRight(), transform->getUp(), transform->getDirection(), collider->getHalfSizes());
    }

    lineIntersectionResult defaultCameraController::lineIntersection(phi::vec2 line1Start, phi::vec2 line1End, phi::vec2 line2Start, phi::vec2 line2End)
    {
        // if the lines intersect, the result contains the x and y of the intersection (treating the lines as infinite) and booleans for whether line segment 1 or line segment 2 contain the point
        lineIntersectionResult result;
        float denominator, a, b, numerator1, numerator2;
        denominator = ((line2End.y - line2Start.y) * (line1End.x - line1Start.x)) - ((line2End.x - line2Start.x) * (line1End.y - line1Start.y));
        if (denominator == 0)
        {
            result.parallel = true;
            return result;
        }

        a = line1Start.y - line2Start.y;
        b = line1Start.x - line2Start.x;
        numerator1 = ((line2End.x - line2Start.x) * a) - ((line2End.y - line2Start.y) * b);
        numerator2 = ((line1End.x - line1Start.x) * a) - ((line1End.y - line1Start.y) * b);
        a = numerator1 / denominator;
        b = numerator2 / denominator;

        // if we cast these lines infinitely in both directions, they intersect here:
        result.point = phi::vec2(line1Start.x + (a * (line1End.x - line1Start.x)), line1Start.y + (a * (line1End.y - line1Start.y)));

        // if line1 is a segment and line2 is infinite, they intersect if:
        if (a > 0 && a < 1)
            result.onLine1 = true;

        // if line2 is a segment and line1 is infinite, they intersect if:
        if (b > 0 && b < 1)
            result.onLine2 = true;

        // if line1 and line2 are segments, they intersect if both of the above are true

        return result;
    };

    contactTestResult defaultCameraController::testContacts(node* testNode, obb obb)
    {
        auto result = contactTestResult();

        auto colliderNodes = _scene->getColliderNodes();
        for (auto node : colliderNodes)
        {
            if (node == testNode)
                continue;

            auto nodeObb = node->getComponent<boxCollider>()->getObb();
            auto intersectionResult = obb::interesects(obb, vec3(), nodeObb, vec3());
            if (intersectionResult.colliding || intersectionResult.intersecting)
                result.contacts.push_back(nodeObb);
        }

        return result;
    }

    continuousContactTestResult defaultCameraController::testContinuousContacts(node* testNode, obb obb, vec3 offset)
    {
        auto result = continuousContactTestResult();

        auto colliderNodes = _scene->getColliderNodes();
        for (auto node : colliderNodes)
        {
            if (node == testNode)
                continue;

            auto nodeObb = node->getComponent<boxCollider>()->getObb();
            auto intersectionResult = obb::interesects(obb, offset, nodeObb, vec3());
            if ((intersectionResult.colliding || intersectionResult.intersecting) && intersectionResult.time >= 0.0f && intersectionResult.time <= 1.0f)
                result.contacts.push_back(continuousCollisionResult(nodeObb, intersectionResult.time, intersectionResult.normal));
        }

        return result;
    }

    void defaultCameraController::moveObject(node* object, vec3 offset, vec3 planeNormal)
    {
        //auto collider = object->getComponent<boxCollider>();

        //auto nodeTransform = object->getTransform();
        //auto rotation = nodeTransform->getLocalOrientation();
        //auto position = nodeTransform->getLocalPosition();
        //auto colliderPosition = collider->getPosition();

        //auto obbStart = collider->getObb();
        //auto obbFinal = obb(obbStart.center + offset, obbStart.axes[0], obbStart.axes[1], obbStart.axes[2], obbStart.halfSizes);

        //auto movementCollisions = testContinuousContacts(object, obbStart, offset).contacts;

        //std::sort(movementCollisions.begin(), movementCollisions.end(), [](const continuousCollisionResult& a, const continuousCollisionResult& b) -> bool
        //{
        //    return a.time > b.time;
        //});

        //bool free = false;
        //auto originalOffset = offset;
        //for (auto collision : movementCollisions)
        //{
        //    auto currentOffsetColision = testContacts(object, obbFinal);

        //    if (currentOffsetColision.contacts.size() > 0)
        //    {
        //        auto dist = glm::dot(collision.normal, planeNormal);
        //        auto normal = glm::normalize(collision.normal - dist * planeNormal);

        //        auto limited = originalOffset * collision.time;
        //        auto up = glm::cross(normal, originalOffset);
        //        auto dotDir = glm::cross(up, normal);
        //        if (dotDir != vec3())
        //            dotDir = glm::normalize(dotDir);

        //        auto dotValue = glm::dot(originalOffset - limited, dotDir);

        //        offset = limited + dotDir * dotValue;
        //        obbFinal = obb(obbStart.center + offset, obbStart.axes[0], obbStart.axes[1], obbStart.axes[2], obbStart.halfSizes);
        //    }
        //    else
        //    {
        //        free = true;
        //        break;
        //    }
        //}

        //auto hehe = [](float v) -> string
        //{
        //    std::stringstream stream;
        //    stream << std::fixed << std::setprecision(30) << v;
        //    return stream.str();
        //};

        //auto truncate = [](float v) -> float
        //{
        //    return glm::round(v * DECIMAL_TRUNCATION_INV) / DECIMAL_TRUNCATION_INV;
        //};

        //if (!free)
        //{
        //    std::sort(movementCollisions.begin(), movementCollisions.end(), [](const continuousCollisionResult& a, const continuousCollisionResult& b) -> bool
        //    {
        //        return a.time < b.time;
        //    });

        //    auto collisionsCount = movementCollisions.size();

        //    for (size_t i = 0; i < collisionsCount && movementCollisions[i].time <= movementCollisions[0].time; i++)
        //    {
        //        auto collisionNormal = movementCollisions[i].normal;
        //        auto collidedObject = movementCollisions[i].obb;
        //        auto time = movementCollisions[i].time;

        //        auto dist = glm::dot(collisionNormal, planeNormal);
        //        auto normal = glm::normalize(collisionNormal - dist * planeNormal);

        //        auto limited = originalOffset * time;
        //        auto up = glm::cross(normal, originalOffset);
        //        auto dotDir = glm::cross(up, normal);
        //        if (dotDir != vec3())
        //            dotDir = glm::normalize(dotDir);

        //        auto dotValue = glm::dot(originalOffset - limited, dotDir);

        //        offset = limited + dotDir * dotValue;
        //        auto obbTest = obb(obbStart.center + limited, obbStart.axes[0], obbStart.axes[1], obbStart.axes[2], obbStart.halfSizes);

        //        // Test all collisions for nearest time for the current movement
        //        //_scene->collisionWorld->convexSweepTest(shape, to, tf, adjustSweepTest, 0.0f);
        //        auto adjustNearestCollision = testContinuousContacts(object, obbTest, dotDir * dotValue).contacts;

        //        if (adjustNearestCollision.size() > 0)
        //        {
        //            auto adjustTime = adjustNearestCollision[0].time;
        //            offset = limited + dotDir * dotValue * adjustTime;
        //            if (adjustTime > 0.0f) // TODO: compare floats with margin?
        //                break;
        //        }
        //        else
        //            break;
        //    }
        //}

        object->getTransform()->translate(offset);
    }

    void defaultCameraController::dragMouseDown(int mouseX, int mouseY)
    {
        if (!_dragObject)
            return;

        auto ray = _camera->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
        phi::aabb* aabb = new phi::aabb();
        _dragObject->traverse
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

        auto model = _dragObject->getTransform()->getModelMatrix();
        auto transformedMin = mathUtils::multiply(model, aabb->min);
        auto transformedMax = mathUtils::multiply(model, aabb->max);
        auto transformedAabb = phi::aabb(transformedMin, transformedMax);
        safeDelete(aabb);

        vec3* positions;
        vec3* normals;
        size_t count;
        if (ray.intersects(transformedAabb, positions, normals, count))
        {
            auto normal = normals[0];

            _dragOrigin = positions[0];
            _dragObjectStartPosition = _dragObject->getTransform()->getLocalPosition();

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

            auto zFightAvoidingOffset = 0.01f;
            if (normal == phi::vec3(-1.0f, 0.0f, 0.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(max.x + zFightAvoidingOffset, 0.0f, 0.0f));
                _dragPlaneBottomLeft = lbb;
                _dragPlaneBottomRight = lbf;
                _dragPlaneTopRight = ltf;
                _dragPlaneTopLeft = ltb;
            }
            else if (normal == phi::vec3(1.0f, 0.0f, 0.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(min.x - zFightAvoidingOffset, 0.0f, 0.0f));
                _dragPlaneBottomLeft = rbf;
                _dragPlaneBottomRight = rbb;
                _dragPlaneTopRight = rtb;
                _dragPlaneTopLeft = rtf;
            }
            else if (normal == phi::vec3(0.0f, 0.0f, 1.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(0.0f, 0.0f, min.z - zFightAvoidingOffset));
                _dragPlaneBottomLeft = lbf;
                _dragPlaneBottomRight = rbf;
                _dragPlaneTopRight = rtf;
                _dragPlaneTopLeft = ltf;
            }
            else if (normal == phi::vec3(0.0f, 0.0f, -1.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(0.0f, 0.0f, max.z + zFightAvoidingOffset));
                _dragPlaneBottomLeft = rbb;
                _dragPlaneBottomRight = lbb;
                _dragPlaneTopRight = ltb;
                _dragPlaneTopLeft = rtb;
            }
            else if (normal == phi::vec3(0.0f, 1.0f, 0.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(0.0f, min.y - zFightAvoidingOffset, 0.0f));
                _dragPlaneBottomLeft = ltf;
                _dragPlaneBottomRight = rtf;
                _dragPlaneTopRight = rtb;
                _dragPlaneTopLeft = ltb;
            }
            else if (normal == phi::vec3(0.0f, -1.0f, 0.0f))
            {
                _planeGridPass->transform.setLocalPosition(phi::vec3(0.0f, max.y + zFightAvoidingOffset, 0.0f));
                _dragPlaneBottomLeft = lbb;
                _dragPlaneBottomRight = rbb;
                _dragPlaneTopRight = rbf;
                _dragPlaneTopLeft = lbf;
            }

            _planeGridPass->transform.setDirection(normal);

            auto planeOrigin = _planeGridPass->transform.getPosition();
            auto planeRight = _planeGridPass->transform.getRight();
            auto planeUp = _planeGridPass->transform.getUp();
            auto point = (_dragPlaneBottomLeft + _dragPlaneTopRight) * 0.5f;

            _planeGridPass->centerPosition = planePointProjection2D(planeOrigin, normal, planeRight, planeUp, point);
            _planeGridPass->show();

            _dragDoingInertia = false;
            _dragging = true;

            //moveObject(_dragObject, phi::vec3(0.07f, 0.07f, 0.0f));

            //auto a = phi::aabb(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
            //auto b = phi::aabb(vec3(0.5f, 0.0f, 0.0f), vec3(1.5f, 1.0f, 1.0f));

            //auto res = phi::aabb::intersects(b, a, vec3(0.0f, 0.0f, 0.0f));

            //if (res.hit.collided)
            //    phi::debug(std::to_string(res.time) + "[" + std::to_string(res.pos.x) + ";" + std::to_string(res.pos.y) + "]");

            //moveObject(_dragObject, vec3(0.0f, -1.0f, 0.0f), normal);
        }
    }

    void defaultCameraController::dragMouseMove()
    {
        auto ray = _camera->screenPointToRay(static_cast<float>(_mousePosX), static_cast<float>(_mousePosY));
        float t;
        ray.intersects(_dragPlaneBottomLeft, _dragPlaneTopLeft, _dragPlaneTopRight, _dragPlaneBottomRight, t);
        auto point = ray.getOrigin() + ray.getDirection() * t;

        auto diff = point - _dragOrigin;
        auto newPosition = _dragObjectStartPosition + diff;

        auto planeOrigin = _planeGridPass->transform.getPosition();
        auto planeNormal = _planeGridPass->transform.getDirection();
        auto planeRight = _planeGridPass->transform.getRight();
        auto planeUp = _planeGridPass->transform.getUp();
        auto centerBoundingBox = (_dragPlaneBottomLeft + _dragPlaneTopRight) * 0.5f + (_dragObject->getTransform()->getPosition() - _dragObjectStartPosition);
        auto projection = planePointProjection2D(planeOrigin, planeNormal, planeRight, planeUp, centerBoundingBox);

        _dragStartPos = _planeGridPass->centerPosition;
        _dragDelta = projection - _dragStartPos;
        _dragDoingInertia = true;
        _dragInertiaTime = 0.0f;

        moveObject(_dragObject, newPosition - _dragObject->getTransform()->getLocalPosition(), planeNormal);
    }

    void defaultCameraController::dragMouseUp()
    {
        if (_dragging)
        {
            _planeGridPass->hide();
            _dragging = false;
        }
    }

    void defaultCameraController::dragUpdate()
    {
        if (!_dragDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _dragInertiaTime += deltaMilliseconds;
        auto percent = -glm::exp(_dragInertiaTime / -100.0f) + 1.0f;
        auto delta = static_cast<float>(glm::length(_dragDelta) * percent);

        if (delta == 0.0f)
            return;

        auto dir = glm::normalize(_dragDelta);
        _planeGridPass->centerPosition = _dragStartPos + dir * delta;

        if (percent >= 1.0f)
            _dragDoingInertia = false;
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

        float z = 10.0;
        if (zBufferValue != 1.0f)
            z = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);

        auto position = _camera->getWorldPositionRelativeToCamera(mouseX, mouseY, z);

        auto cameraTransform = _camera->getTransform();
        auto camDir = cameraTransform->getDirection();
        auto camRight = cameraTransform->getRight();
        auto camUp = cameraTransform->getUp();
        auto zNear = _camera->getNear();

        _zoomDir = glm::normalize(
            camDir * z +
            -camRight * position.x +
            camUp * position.y);

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

        if (zBufferValue == 1.0f)
            _panEyeZ = 20.0f;
        else
            _panEyeZ = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);

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
        _panDelta += _panDelta * (1.0f - glm::clamp(deltaTime / 100.0f, 0.0f, 1.0f));
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

        if (zBufferValue == 1.0f)
            _rotationTargetPos = glm::vec3();
        else
        {
            auto worldZ = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);
            auto worldPosition = _camera->getWorldPositionRelativeToCamera(mouseX, mouseY, worldZ);

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _rotationTargetPos =
                camPos +
                (-camRight * worldPosition.x) +
                (camUp * worldPosition.y) +
                (camDir * worldPosition.z);
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