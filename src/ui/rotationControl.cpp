#include <phi/rendering/shaderManager.h>

#include <phi/ui/rotationControl.h>
#include <phi/ui/colorAnimator.h>

#include <glm/gtx/constants.hpp>

namespace phi
{
    rotationControl::rotationControl(size<GLuint> viewportSize) :
        control(viewportSize)
    {
        _shader = shaderManager::get()->getShader("UI_MESH");
        _object = nullptr;
        _xModelMatrix = glm::rotate(glm::pi<float>() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        _yModelMatrix = glm::rotate(glm::pi<float>() * -0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        _zModelMatrix = glm::mat4();
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
        _xPositions = std::vector<glm::vec3>();
        _yPositions = std::vector<glm::vec3>();
        _zPositions = std::vector<glm::vec3>();
        auto indices = new std::vector<GLuint>();
        auto r = 0.7f;
        auto n = 50;
        auto as = (glm::pi<float>() * 2.0f) / (float)n;
        auto a = 0.0f;

        auto multMat3 = [] (const glm::vec3 vec, const glm::mat4 mat)
        {
            auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
            vec4 = mat * vec4;
            return glm::vec3(vec4.x, vec4.y, vec4.z);
        };

        for (auto i = 0; i < n; i++)
        {
            auto pos = glm::vec3(glm::cos(a) * r, glm::sin(a) * r, 0.0f);
            _xPositions.push_back(multMat3(pos, _xModelMatrix));
            _yPositions.push_back(multMat3(pos, _yModelMatrix));
            _zPositions.push_back(pos);
            indices->push_back(i);
            indices->push_back((i + 1) % n);
            a += as;
        }

        _circleMesh = lineMesh::create("hi!", _zPositions, indices);
    }

    void rotationControl::updateModelMatrix()
    {
        if (!_object)
            return;

        auto viewModelFixed = _camera->getViewMatrix() * _object->getModelMatrix();
        auto x = viewModelFixed[3][0];
        auto y = viewModelFixed[3][1];
        auto z = viewModelFixed[3][2];
        auto px = x / z;
        auto py = y / z;
        viewModelFixed[3][0] = -1.0f * px;
        viewModelFixed[3][1] = -1.0f * py;
        viewModelFixed[3][2] = -1.0f;
        auto pos = glm::inverse(_camera->getViewMatrix()) * viewModelFixed * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        auto rot = glm::mat4();
        auto obj = _object;
        while (obj != nullptr)
        {
            rot = obj->getRotationMatrix() * rot;
            obj = obj->getParent();
        }

        auto translationMatrix = glm::translate(glm::vec3(pos.x, pos.y, pos.z)) * rot * glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));

        _modelMatrix = translationMatrix;
    }

    float minimum_distance(glm::vec2 v, glm::vec2 w, glm::vec2 p)
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
        const glm::vec2 projection = v + t * (w - v);  // Projection falls on the segment
        return glm::length(projection - p);
    }

#define SMALL_NUM   0.00000001 // anything that avoids division overflow

    // intersect3D_SegmentPlane(): find the 3D intersection of a segment and a plane
    //    Input:  S = a segment, and Pn = a plane = {Point V0;  Vector n;}
    //    Output: *I0 = the intersect point (when it exists)
    //    Return: 0 = disjoint (no intersection)
    //            1 =  intersection in the unique point *I0
    //            2 = the  segment lies in the plane
    int intersect3D_SegmentPlane(glm::vec3 rayStart, glm::vec3 rayEnd, glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3* intersection)
    {
        glm::vec3 u = rayEnd - rayStart;
        glm::vec3 w = rayStart - planePoint;

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
        auto hh = _viewportSize.height * 0.5f;
        auto hw = _viewportSize.width * 0.5f;

        auto vp = _camera->getPerspProjMatrix() * _camera->getViewMatrix() * _modelMatrix;

        auto worldToScreen = [&] (const glm::vec3 vec)
        {
            auto v = glm::vec2(hw, hh);
            auto pos = glm::vec4(vec, 1.0);
            pos = vp * pos;
            pos = pos / pos.w;
            return glm::vec2(pos.x, -pos.y) * v + v;
        };

        auto count = _xPositions.size();
        auto mousePosition = glm::vec2(x, y);

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
        if (!_object)
            return;

        if (e->leftButtonPressed && (_mouseOverX || _mouseOverY || _mouseOverZ))
        {
            _clickedOverX = _mouseOverX;
            _clickedOverY = _mouseOverY;
            _clickedOverZ = _mouseOverZ;
            _mouseStartPos = glm::vec2(e->x, e->y);
            _currentAngle = 0.0f;
            _startOrientation = _object->getOrientation();
            e->handled = true;
        }
    }

    void rotationControl::onMouseUp(mouseEventArgs* e)
    {
        if (!_object)
            return;

        if (e->leftButtonPressed && (_clickedOverX || _clickedOverY || _clickedOverZ))
        {
            _clickedOverX = _clickedOverY = _clickedOverZ = false;
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

            if (_rotationFinished->isBound())
                _rotationFinished->invoke(phi::rotationEventArgs(_object, _startOrientation, _object->getOrientation()));
        }
    }

    glm::vec3 _a;
    glm::vec3 _b;

    void rotationControl::onMouseMove(mouseEventArgs* e)
    {
        if (!_object)
            return;

        auto multMat3 = [] (const glm::vec3 vec, const glm::mat4 mat)
        {
            auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
            vec4 = mat * vec4;
            return glm::vec3(vec4.x, vec4.y, vec4.z);
        };

        auto screenToViewZNear = [&] (const glm::vec2 vec)
        {
            float zNear = _camera->getFrustum()->getZNear();
            float aspect = _camera->getFrustum()->getAspect();
            float fov = _camera->getFrustum()->getFov();

            float tg = glm::tan(fov * 0.5f) * zNear;

            float h = _viewportSize.height;
            float w = _viewportSize.width;

            float hh = h * 0.5f;
            float hw = w * 0.5f;

            float ys = vec.y - hh;
            float yp = ys/hh;
            float y = -(yp * tg);

            float xs = vec.x - hw;
            float xp = xs/hw;
            float x = xp * tg * aspect;

            return glm::vec3(x, y, -zNear);
        };

        auto worldToScreen = [&] (const glm::vec3 worldPos)
        {
            auto vp = _camera->getPerspProjMatrix() * _camera->getViewMatrix();
            auto vw = _viewportSize.width * 0.5f;
            auto vh = _viewportSize.height * 0.5f;
            auto v = glm::vec2(vw, vh);
            auto pos = glm::vec4(worldPos, 1.0);
            pos = vp * pos;
            pos = pos / pos.w;
            return glm::vec2(pos.x, -pos.y) * v + v;
        };

        auto worldToView = [&] (const glm::vec3 vec)
        {
            auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
            vec4 = _camera->getViewMatrix() * vec4;
            return glm::vec3(vec4.x, vec4.y, vec4.z);
        };

        if (e->x == _mouseStartPos.x && e->y == _mouseStartPos.y)
            return;

        if (_clickedOverX || _clickedOverY || _clickedOverZ)
        {
            auto rot = glm::mat4();
            auto obj = _object;
            while (obj != nullptr)
            {
                rot = obj->getRotationMatrix() * rot;
                obj = obj->getParent();
            }

            glm::vec3 dir, dirLocal;
            if (_clickedOverX)
            {
                dir = glm::normalize(multMat3(glm::vec3(1.0f, 0.0f, 0.0f), rot));
                dirLocal = glm::normalize(multMat3(glm::vec3(1.0f, 0.0f, 0.0f), _object->getRotationMatrix()));
            }
            if (_clickedOverY)
            {
                dir = glm::normalize(multMat3(glm::vec3(0.0f, 1.0f, 0.0f), rot));
                dirLocal = glm::normalize(multMat3(glm::vec3(0.0f, 1.0f, 0.0f), _object->getRotationMatrix()));
            }
            if (_clickedOverZ)
            {
                dir = glm::normalize(multMat3(glm::vec3(0.0f, 0.0f, 1.0f), rot));
                dirLocal = glm::normalize(multMat3(glm::vec3(0.0f, 0.0f, 1.0f), _object->getRotationMatrix()));
            }

            auto objectPosition = multMat3(glm::vec3(), _object->getModelMatrix());
            auto planePoint = objectPosition;
            auto planeNormal = dir;
            auto rayStart = _camera->getPosition();
            auto rayEnd = multMat3(screenToViewZNear(glm::vec2(e->x, e->y)), glm::inverse(_camera->getViewMatrix()));
            auto rayEndStart = multMat3(screenToViewZNear(_mouseStartPos), glm::inverse(_camera->getViewMatrix()));

            glm::vec3 intersection, intersectionStart;
            auto b = intersect3D_SegmentPlane(rayStart, rayEnd, planePoint, planeNormal, &intersection);
            //LOG("rayStart[" + std::to_string(rayStart.x) + ";" + std::to_string(rayStart.y) + ";" + std::to_string(rayStart.z) + "]" + 
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

                auto currentRot = _object->getOrientation();
                _object->rotate(angle - _currentAngle, dirLocal);

                auto args = new rotationEventArgs(_object, currentRot, _object->getOrientation());
                if (_rotating->isBound())
                    _rotating->invoke(args);

                if (args->cancel)
                    _object->setOrientation(currentRot);
                else
                    _currentAngle = angle;

                DELETE(args);
            }
        }
    }

    void rotationControl::onMouseLeave(mouseEventArgs* e)
    {
        if (!_object)
            return;

        //colorAnimator::animateColor(&_xColor, color(0.7f, 0.0f, 0.0f), 300);
        //colorAnimator::animateColor(&_yColor, color(0.0f, 0.7f, 0.0f), 300);
        //colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 0.7f), 300);
    }

    void rotationControl::attachTo(object3D* object)
    {
        _object = object;
    }

    void rotationControl::onRender()
    {
        if (!_object)
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
        //auto aPos = std::vector<glm::vec3>();
        //aPos.push_back(_object->getPosition());
        //aPos.push_back(_a);
        //auto bPos = std::vector<glm::vec3>();
        //bPos.push_back(_object->getPosition());
        //bPos.push_back(_b);
        //auto ind = std::vector<GLuint>();
        //ind.push_back(0);
        //ind.push_back(1);
        //auto a = lineMesh::create("oi", aPos, ind);
        //a->bind();
        //a->render();
        //a->unbind();
        //auto b = lineMesh::create("oi", bPos, ind);
        //b->bind();
        //b->render();
        //b->unbind();
        //DELETE(a);
        //DELETE(b);
        //_shader->unbind();
    }

    void rotationControl::renderCircle(color color, glm::mat4 modelMatrix)
    {
        auto mvp = _camera->getPerspProjMatrix() * _camera->getViewMatrix() * _modelMatrix * modelMatrix;
        _shader->bind();
        _shader->setUniform("mvp", mvp);
        _shader->setUniform("color", color);
        _circleMesh->bind();
        _circleMesh->render();
        _circleMesh->unbind();
        _shader->unbind();
    }
}