#include <precompiled.h>

#include <rendering\shadersManager.h>

#include "rotationControl.h"
#include "colorAnimator.h"

namespace phi
{
    rotationControl::rotationControl(sizeui viewportSize) :
        control(viewportSize)
    {
        //_shader = shaderManager::get()->getShader("UI_MESH");
        _transform = nullptr;
        _xModelMatrix = glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
        _yModelMatrix = glm::rotate(glm::pi<float>() * -0.5f, vec3(1.0f, 0.0f, 0.0f));
        _zModelMatrix = mat4();
        _clickedOverX = _clickedOverY = _clickedOverZ = _mouseOverX = _mouseOverY = _mouseOverZ = false;
        _xColor = color(1.0f, 0.0f, 0.0f, 0.5f);
        _yColor = color(0.0f, 1.0f, 0.0f, 0.5f);
        _zColor = color(0.0f, 0.0f, 1.0f, 0.5f);
        _rotating = new eventHandler<rotationEventArgs*>();
        _rotationFinished = new eventHandler<rotationEventArgs>();
        createCircleMesh();
    }

    void rotationControl::createCircleMesh()
    {
        _xPositions = std::vector<vec3>();
        _yPositions = std::vector<vec3>();
        _zPositions = std::vector<vec3>();
        vector<GLuint> indices;

        auto r = 0.7f;
        auto n = 50;
        auto as = (glm::pi<float>() * 2.0f) / (float)n;
        auto a = 0.0f;

        for (auto i = 0; i < n; i++)
        {
            auto pos = vec3(glm::cos(a) * r, glm::sin(a) * r, 0.0f);
            _xPositions.push_back(mathUtils::multiply(_xModelMatrix, pos));
            _yPositions.push_back(mathUtils::multiply(_yModelMatrix, pos));
            _zPositions.push_back(pos);
            indices.push_back(i);
            indices.push_back((i + 1) % n);
            a += as;
        }

        _circleMesh = lineMesh::create("hi!", _zPositions, indices);
    }

    void rotationControl::updateModelMatrix()
    {
        if (!_transform)
            return;

        auto viewModelFixed = _camera->getViewMatrix() * _transform->getModelMatrix();
        auto x = viewModelFixed[3][0];
        auto y = viewModelFixed[3][1];
        auto z = viewModelFixed[3][2];
        auto px = x / z;
        auto py = y / z;
        viewModelFixed[3][0] = -1.0f * px;
        viewModelFixed[3][1] = -1.0f * py;
        viewModelFixed[3][2] = -1.0f;
        auto pos = glm::inverse(_camera->getViewMatrix()) * viewModelFixed * vec4(0.0f, 0.0f, 0.0f, 1.0f);

        auto rot = mat4();
        auto obj = _transform; // wtf?
        while (obj != nullptr)
        {
            rot = obj->getLocalRotationMatrix() * rot;
            obj = obj->getParent();
        }

        auto translationMatrix = glm::translate(vec3(pos.x, pos.y, pos.z)) * rot * glm::scale(vec3(0.15f, 0.15f, 0.15f));

        _modelMatrix = translationMatrix;
    }

    float minimum_distance(vec2 v, vec2 w, vec2 p)
    {
        // Return minimum distance between line segment vw and point p
        const float l2 = glm::length2(w - v);  // i.e. |w-v|^2 -  avoid a sqrt
        if (l2 == 0.0)
            return glm::length(v - p);   // v == w case

        // Consider the line extending the segment, parameterized as v + t (w - v).
        // We find projection of point p onto the line. 
        // It falls where t = [(p-v) . (w-v)] / |w-v|^2
        const float t = glm::dot(p - v, w - v) / l2;
        if (t < 0.0) return glm::length(v - p);       // Beyond the 'v' end of the segment
        else if (t > 1.0) return glm::length(w - p);  // Beyond the 'w' end of the segment
        const vec2 projection = v + t * (w - v);  // Projection falls on the segment
        return glm::length(projection - p);
    }

#define SMALL_NUM   0.00000001 // anything that avoids division overflow

    // intersect3D_SegmentPlane(): find the 3D intersection of a segment and a plane
    //    Input:  S = a segment, and Pn = a plane = {Point V0;  Vector n;}
    //    Output: *I0 = the intersect point (when it exists)
    //    Return: 0 = disjoint (no intersection)
    //            1 =  intersection in the unique point *I0
    //            2 = the  segment lies in the plane
    int intersect3D_SegmentPlane(vec3 rayStart, vec3 rayEnd, vec3 planePoint, vec3 planeNormal, vec3* intersection)
    {
        vec3 u = rayEnd - rayStart;
        vec3 w = rayStart - planePoint;

        float D = glm::dot(planeNormal, u);
        float N = -glm::dot(planeNormal, w);

        if (fabs(D) < SMALL_NUM) // segment is parallel to plane
        {
            if (N == 0) // segment lies in plane
                return 2;
            else
                return 0; // no intersection
        }
        // they are not parallel
        // compute intersect param
        float sI = N / D;
        //if (sI < 0 || sI > 1)
        //return 0; // no intersection

        *intersection = rayStart + sI * u; // compute segment intersect point
        return 1;
    }

    bool rotationControl::isPointInside(int x, int y)
    {
        auto hh = _viewportSize.h * 0.5f;
        auto hw = _viewportSize.w * 0.5f;

        auto vp = _camera->getProjectionMatrix() * _camera->getViewMatrix() * _modelMatrix;

        auto worldToScreen = [&] (const vec3 vec)
        {
            auto v = vec2(hw, hh);
            auto pos = vec4(vec, 1.0);
            pos = vp * pos;
            pos = pos / pos.w;
            return vec2(pos.x, -pos.y) * v + v;
        };

        auto count = _xPositions.size();
        auto mousePosition = vec2(x, y);

        auto mouseOverXOld = _mouseOverX;
        auto mouseOverYOld = _mouseOverY;
        auto mouseOverZOld = _mouseOverZ;

        _mouseOverX = _mouseOverY = _mouseOverZ = false;

        for (auto i = 0; i < count; i++)
        {
            auto screenA = worldToScreen(_xPositions[i]);
            auto screenB = worldToScreen(_xPositions[(i + 1) % count]);

            auto dist = minimum_distance(screenA, screenB, mousePosition);
            if (dist < 5)
            {
                _mouseOverX = true;
                break;
            }
        }

        if (!mouseOverXOld && _mouseOverX)
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 1.0f), 300);
        if (mouseOverXOld && !_mouseOverX && !_clickedOverX)
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);

        if (!_mouseOverX)
        {
            for (auto i = 0; i < count; i++)
            {
                auto screenA = worldToScreen(_yPositions[i]);
                auto screenB = worldToScreen(_yPositions[(i + 1) % count]);

                auto dist = minimum_distance(screenA, screenB, mousePosition);
                if (dist < 5)
                {
                    _mouseOverY = true;
                    break;
                }
            }
        }

        if (!mouseOverYOld && _mouseOverY)
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 1.0f), 300);
        if (mouseOverYOld && !_mouseOverY && !_clickedOverY)
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);

        if (!_mouseOverX && !_mouseOverY)
        {
            for (auto i = 0; i < count; i++)
            {
                auto screenA = worldToScreen(_zPositions[i]);
                auto screenB = worldToScreen(_zPositions[(i + 1) % count]);

                auto dist = minimum_distance(screenA, screenB, mousePosition);
                if (dist < 5)
                {
                    _mouseOverZ = true;
                    break;
                }
            }
        }

        if (!mouseOverZOld && _mouseOverZ)
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 1.0f), 300);
        if (mouseOverZOld && !_mouseOverZ && !_clickedOverZ)
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

        return false;
    }

    void rotationControl::onMouseDown(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        if (e->leftButtonPressed && (_mouseOverX || _mouseOverY || _mouseOverZ))
        {
            _clickedOverX = _mouseOverX;
            _clickedOverY = _mouseOverY;
            _clickedOverZ = _mouseOverZ;
            _mouseStartPos = vec2(e->x, e->y);
            _currentAngle = 0.0f;
            _startOrientation = _transform->getLocalOrientation();
            e->handled = true;
        }
    }

    void rotationControl::onMouseUp(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        if (e->leftButtonPressed && (_clickedOverX || _clickedOverY || _clickedOverZ))
        {
            _clickedOverX = _clickedOverY = _clickedOverZ = false;
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

            _rotationFinished->raise(phi::rotationEventArgs(_transform, _startOrientation, _transform->getLocalOrientation()));
        }
    }

    vec3 _a;
    vec3 _b;

    void rotationControl::onMouseMove(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        auto screenToViewZNear = [&] (const vec2 vec)
        {
            auto zNear = _camera->getNear();
            auto aspect = _camera->getAspect();
            auto fov = _camera->getFov();

            auto tg = tan(fov * 0.5f) * zNear;

            auto h = _viewportSize.h;
            auto w = _viewportSize.w;

            auto hh = h * 0.5f;
            auto hw = w * 0.5f;

            auto ys = vec.y - hh;
            auto yp = ys/hh;
            auto y = -(yp * tg);

            auto xs = vec.x - hw;
            auto xp = xs/hw;
            auto x = xp * tg * aspect;

            return vec3(x, y, -zNear);
        };

        auto worldToScreen = [&] (const vec3 worldPos)
        {
            auto vp = _camera->getProjectionMatrix() * _camera->getViewMatrix();
            auto vw = _viewportSize.w * 0.5f;
            auto vh = _viewportSize.h * 0.5f;
            auto v = vec2(vw, vh);
            auto pos = vec4(worldPos, 1.0);
            pos = vp * pos;
            pos = pos / pos.w;
            return vec2(pos.x, -pos.y) * v + v;
        };

        auto worldToView = [&] (const vec3 vec)
        {
            auto v = vec4(vec.x, vec.y, vec.z, 1.0f);
            v = _camera->getViewMatrix() * v;
            return vec3(v.x, v.y, v.z);
        };

        if (e->x == _mouseStartPos.x && e->y == _mouseStartPos.y)
            return;

        if (_clickedOverX || _clickedOverY || _clickedOverZ)
        {
            auto rot = mat4();
            auto obj = _transform;
            while (obj != nullptr)
            {
                rot = obj->getLocalRotationMatrix() * rot;
                obj = obj->getParent();
            }

            vec3 dir, dirLocal;
            if (_clickedOverX)
            {
                dir = glm::normalize(mathUtils::multiply(rot, vec3(1.0f, 0.0f, 0.0f)));
                dirLocal = glm::normalize(mathUtils::multiply(_transform->getLocalRotationMatrix(), vec3(1.0f, 0.0f, 0.0f)));
            }
            if (_clickedOverY)
            {
                dir = glm::normalize(mathUtils::multiply(rot, vec3(0.0f, 1.0f, 0.0f)));
                dirLocal = glm::normalize(mathUtils::multiply(_transform->getLocalRotationMatrix(), vec3(0.0f, 1.0f, 0.0f)));
            }
            if (_clickedOverZ)
            {
                dir = glm::normalize(mathUtils::multiply(rot, vec3(0.0f, 0.0f, 1.0f)));
                dirLocal = glm::normalize(mathUtils::multiply(_transform->getLocalRotationMatrix(), vec3(0.0f, 0.0f, 1.0f)));
            }

            auto objectPosition = mathUtils::multiply(_transform->getModelMatrix(), vec3());
            auto planePoint = objectPosition;
            auto planeNormal = dir;
            auto rayStart = _camera->getTransform()->getLocalPosition();
            auto rayEnd = mathUtils::multiply(glm::inverse(_camera->getViewMatrix()), screenToViewZNear(vec2(e->x, e->y)));
            auto rayEndStart = mathUtils::multiply(glm::inverse(_camera->getViewMatrix()), screenToViewZNear(_mouseStartPos));

            vec3 intersection, intersectionStart;
            auto b = intersect3D_SegmentPlane(rayStart, rayEnd, planePoint, planeNormal, &intersection);
            //log("rayStart[" + std::to_string(rayStart.x) + ";" + std::to_string(rayStart.y) + ";" + std::to_string(rayStart.z) + "]" + 
            //    "rayEnd[" + std::to_string(rayEnd.x) + ";" + std::to_string(rayEnd.y) + ";" + std::to_string(rayEnd.z) + "]" + 
            //    "planePoint[" + std::to_string(planePoint.x) + ";" + std::to_string(planePoint.y) + ";" + std::to_string(planePoint.z) + "]" + 
            //    "planePoint[" + std::to_string(planeNormal.x) + ";" + std::to_string(planeNormal.y) + ";" + std::to_string(planeNormal.z) + "]");

            auto c = intersect3D_SegmentPlane(rayStart, rayEndStart, planePoint, planeNormal, &intersectionStart);
            if (b > 0 && c > 0)
            {
                _a = intersection;
                _b = intersectionStart;

                intersection = glm::normalize(intersection - objectPosition);
                intersectionStart = glm::normalize(intersectionStart - objectPosition);

                auto angle = glm::acos(glm::dot(intersection, intersectionStart));
                auto sign = glm::sign(glm::dot(-dir, glm::cross(intersection, intersectionStart)));
                // angle in [-179,180]
                angle = angle * sign;

                auto currentRot = _transform->getLocalOrientation();
                _transform->rotate(angle - _currentAngle, dirLocal);

                auto args = new rotationEventArgs(_transform, currentRot, _transform->getLocalOrientation());
                _rotating->raise(args);

                if (args->cancel)
                    _transform->setLocalOrientation(currentRot);
                else
                    _currentAngle = angle;

                safeDelete(args);
            }
        }
    }

    void rotationControl::onMouseLeave(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        //colorAnimator::animateColor(&_xColor, color(0.7f, 0.0f, 0.0f), 300);
        //colorAnimator::animateColor(&_yColor, color(0.0f, 0.7f, 0.0f), 300);
        //colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 0.7f), 300);
    }

    void rotationControl::attachTo(transform* transform)
    {
        _transform = transform;
    }

    void rotationControl::onRender()
    {
        if (!_transform)
            return;

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth((GLfloat)2.5f);

        updateModelMatrix();
        renderCircle(_xColor, _xModelMatrix);
        renderCircle(_yColor, _yModelMatrix);
        renderCircle(_zColor, _zModelMatrix);

        glLineWidth((GLfloat)1.0f);

        //auto mvp = _camera->getPerspProjMatrix() * _camera->getViewMatrix();
        //_shader->bind();
        //_shader->setUniform("mvp", mvp);
        //_shader->setUniform("color", color::white);
        //auto aPos = std::vector<vec3>();
        //aPos.push_back(_object->getPosition());
        //aPos.push_back(_a);
        //auto bPos = std::vector<vec3>();
        //bPos.push_back(_object->getPosition());
        //bPos.push_back(_b);
        //auto ind = std::vector<GLuint>();
        //ind.push_back(0);
        //ind.push_back(1);
        //auto a = linegeometry::create("oi", aPos, ind);
        //a->bind();
        //a->render();
        //a->unbind();
        //auto b = linegeometry::create("oi", bPos, ind);
        //b->bind();
        //b->render();
        //b->unbind();
        //DELETE(a);
        //DELETE(b);
        //_shader->unbind();
    }

    void rotationControl::renderCircle(color color, mat4 modelMatrix)
    {
        auto mvp = _camera->getProjectionMatrix() * _camera->getViewMatrix() * _modelMatrix * modelMatrix;
        _shader->bind();
        //_shader->setUniform("mvp", mvp);
        //_shader->setUniform("color", color);
        _circleMesh->bind();
        _circleMesh->render();
        _circleMesh->unbind();
        _shader->unbind();
    }
}