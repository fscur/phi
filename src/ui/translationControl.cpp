#include <precompiled.h>
#include "translationControl.h"

#include <rendering\shadersManager.h>
//#include <rendering\geometryRenderer.h>
#include <rendering\lineMesh.h>

#include "colorAnimator.h"

namespace phi
{
    translationControl::translationControl(sizef viewportSize) :
        control(viewportSize)
    {
        _arrowGeometry = createArrowGeometry();
        //_shader = shaderManager::get()->getShader("UI_MESH");
        _transform = nullptr;
        _xAabb = new aabb(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
        _yAabb = new aabb(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
        _zAabb = new aabb(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
        _xColor = color::fromRGBA(1.0f, 0.0f, 0.0f, 0.5f);
        _yColor = color::fromRGBA(0.0f, 1.0f, 0.0f, 0.5f);
        _zColor = color::fromRGBA(0.0f, 0.0f, 1.0f, 0.5f);
        _xModelMatrix = glm::rotate(PI * 0.5f, vec3(0.0f, 1.0f, 0.0f));
        _yModelMatrix = glm::rotate(PI * -0.5f, vec3(1.0f, 0.0f, 0.0f));
        _zModelMatrix = mat4();
        _clickedOverX = _clickedOverY = _clickedOverZ = false;
        _translating = new eventHandler<translationEventArgs*>();
        _translationFinished = new eventHandler<translationEventArgs*>();
    }

    geometry* translationControl::createArrowGeometry()
    {
        std::vector<vertex> vertices;
        auto indices = new std::vector<GLuint>();

        auto sides = 10;
        auto pr = 0.05f;
        auto sr = 0.02f;
        auto ap = (PI * 2.0f) / (float)sides;

        vertices.push_back(vec3(0.0f, 0.0f, 0.5f)); // 0

        for (auto i = 0; i < sides; i++)
            vertices.push_back(vec3(glm::cos(ap * i) * pr, glm::sin(ap * i) * pr, 0.35f));

        for (auto i = 0; i < sides; i++)
            vertices.push_back(vec3(glm::cos(ap * i) * sr, glm::sin(ap * i) * sr, 0.35f));

        for (auto i = 0; i < sides; i++)
            vertices.push_back(vec3(glm::cos(ap * i) * sr, glm::sin(ap * i) * sr, 0.0f));

        vertices.push_back(vec3(0.0f, 0.0f, 0.0f));

        for (auto i = 1; i < sides + 1; i++)
        {
            indices->push_back(i);
            indices->push_back(i % sides + 1);
            indices->push_back(0);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices->push_back(i);
            indices->push_back(i + sides);
            indices->push_back(i % sides + 1);

            indices->push_back(i % sides + 1);
            indices->push_back(i + sides);
            indices->push_back(i % sides + sides + 1);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices->push_back(i + sides);
            indices->push_back(i + sides + sides);
            indices->push_back(i % sides + 1 + sides);

            indices->push_back(i % sides + 1 + sides);
            indices->push_back(i + sides + sides);
            indices->push_back(i % sides + sides + 1 + sides);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices->push_back(i + sides * 2);
            indices->push_back(sides * 3 + 1);
            indices->push_back(i % sides + 1 + sides * 2);
        }

        geometry::calcTangents(vertices, *indices);

        //auto min = vertices[0].getPosition();
        //auto max = vertices[0].getPosition();
        //for (auto i = 1; i < 16; i++)
        //{
        //    auto pos = vertices[i].getPosition();
        //    if (pos.x < min.x)
        //        min.x = pos.x;
        //    else if (pos.x > max.x)
        //        max.x = pos.x;

        //    if (pos.y < min.y)
        //        min.y = pos.y;
        //    else if (pos.y > max.y)
        //        max.y = pos.y;

        //    if (pos.z < min.z)
        //        min.z = pos.z;
        //    else if (pos.z > max.z)
        //        max.z = pos.z;
        //}

        //return geometry::create("arrow", vertices, indices);
        return nullptr;
    }

    void translationControl::updateModelMatrix()
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
        auto pos = inverse(_camera->getViewMatrix()) * viewModelFixed * vec4(0.0f, 0.0f, 0.0f, 1.0f);

        auto rot = mat4();
        auto obj = _transform;
        while (obj != nullptr)
        {
            rot = obj->getLocalRotationMatrix() * rot;
            obj = obj->getParent();
        }

        auto translationMatrix = translate(vec3(pos.x, pos.y, pos.z)) * rot * scale(vec3(0.15f, 0.15f, 0.15f));

        _modelMatrix = translationMatrix;
    }

    void translationControl::updateAabbs()
    {
        if (!_transform)
            return;

        auto min = vec4(-0.80901699437494742410229341718282f * 0.05f, -0.95105651629515357211643933337938f * 0.05f, 0.0f, 1.0f);
        auto max = vec4(1.0f * 0.05f, 0.95105651629515357211643933337938f * 0.05f, 0.5f, 1.0f);

        auto createAabb = [&](const mat4 modelMatrix)
        {
            auto newMin = _modelMatrix * modelMatrix * min;
            auto newMax = _modelMatrix * modelMatrix * max;
            return new aabb(vec3(newMin.x, newMin.y, newMin.z), vec3(newMax.x, newMax.y, newMax.z));
        };

        _xAabb = createAabb(_xModelMatrix);
        _yAabb = createAabb(_yModelMatrix);
        _zAabb = createAabb(_zModelMatrix);
    }

    ray translationControl::castRay(vec2 screenCoords, sizef screenSize)
    {
        float w = (float)screenSize.w;
        float h = (float)screenSize.h;
        float x = (2 * screenCoords.x) / w - 1.0f;
        float y = 1.0f - (2 * screenCoords.y) / h;

        mat4 invPersp = inverse(_camera->getProjectionMatrix());
        mat4 invView = inverse(_camera->getViewMatrix());

        vec4 ray_clip = vec4(x, y, -1.0f, 1.0f);
        vec4 ray_eye = invPersp * ray_clip;
        ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
        vec3 ray_world = vec3(invView * ray_eye);
        ray_world = normalize(ray_world);

        return ray(_camera->getTransform()->getLocalPosition(), ray_world);
    }

    bool translationControl::isPointInside(int x, int y)
    {
        if (!_transform)
            return false;

        auto ray = castRay(vec2(x, y), _viewportSize);
        auto xMouseOverOld = _mouseOverX;
        _mouseOverX = ray.intersects(_xAabb);
        if (!xMouseOverOld && _mouseOverX)
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 1.0f), 300);
        if (xMouseOverOld && !_mouseOverX && !_clickedOverX)
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);

        auto yMouseOverOld = _mouseOverY;
        _mouseOverY = ray.intersects(_yAabb) && !_mouseOverX;
        if (!yMouseOverOld && _mouseOverY)
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 1.0f), 300);
        if (yMouseOverOld && !_mouseOverY && !_clickedOverY)
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);

        auto zMouseOverOld = _mouseOverZ;
        _mouseOverZ = ray.intersects(_zAabb) && !_mouseOverX && !_mouseOverY;
        if (!zMouseOverOld && _mouseOverZ)
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 1.0f), 300);
        if (zMouseOverOld && !_mouseOverZ && !_clickedOverZ)
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

        return _mouseOverX || _mouseOverY || _mouseOverZ;
    }

    vec3 translationControl::screenToViewZNear(vec2 mousePos)
    {
        auto zNear = _camera->getZNear();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto h = _viewportSize.h;
        auto w = _viewportSize.w;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys = mousePos.y - hh;
        auto yp = ys / hh;
        auto y = -(yp * tg);

        auto xs = mousePos.x - hw;
        auto xp = xs / hw;
        auto x = xp * tg * aspect;

        return vec3(x, y, -zNear);
    }

    vec2 translationControl::worldToScreen(vec3 worldPos)
    {
        auto vp = _camera->getProjectionMatrix() * _camera->getViewMatrix();
        auto vw = _viewportSize.w * 0.5f;
        auto vh = _viewportSize.h * 0.5f;
        auto v = vec2(vw, vh);
        auto pos = vec4(worldPos, 1.0);
        pos = vp * pos;
        pos = pos / pos.w;
        return vec2(pos.x, -pos.y) * v + v;
    }

    int translationControl::lineLineIntersect(vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 *pa, vec3 *pb, float *mua, float *mub)
    {
        vec3 p13, p43, p21;
        float d1343, d4321, d1321, d4343, d2121;
        float numer, denom;
        float epsilon = glm::epsilon<float>();

        p13.x = p1.x - p3.x;
        p13.y = p1.y - p3.y;
        p13.z = p1.z - p3.z;
        p43.x = p4.x - p3.x;
        p43.y = p4.y - p3.y;
        p43.z = p4.z - p3.z;
        if (abs(p43.x) < epsilon && abs(p43.y) < epsilon && abs(p43.z) < epsilon)
            return false;
        p21.x = p2.x - p1.x;
        p21.y = p2.y - p1.y;
        p21.z = p2.z - p1.z;
        if (abs(p21.x) < epsilon && abs(p21.y) < epsilon && abs(p21.z) < epsilon)
            return false;

        d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
        d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
        d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
        d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
        d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

        denom = d2121 * d4343 - d4321 * d4321;
        if (abs(denom) < epsilon)
            return false;

        numer = d1343 * d4321 - d1321 * d4343;

        *mua = numer / denom;
        *mub = (d1343 + d4321 * (*mua)) / d4343;

        pa->x = p1.x + *mua * p21.x;
        pa->y = p1.y + *mua * p21.y;
        pa->z = p1.z + *mua * p21.z;
        pb->x = p3.x + *mub * p43.x;
        pb->y = p3.y + *mub * p43.y;
        pb->z = p3.z + *mub * p43.z;

        return true;
    }

    void translationControl::onMouseDown(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        if (e->leftButtonPressed && (_mouseOverX || _mouseOverY || _mouseOverZ))
        {
            _clickedOverX = _mouseOverX;
            _clickedOverY = _mouseOverY;
            _clickedOverZ = _mouseOverZ;
            _startPos = _transform->getPosition();
            _startLocalPos = _transform->getLocalPosition();
            _mouseStartPos = vec2(e->x, e->y);
            e->handled = true;
        }
    }

    void translationControl::onMouseUp(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        if (e->leftButtonPressed && (_clickedOverX || _clickedOverY || _clickedOverZ))
        {
            _clickedOverX = _clickedOverY = _clickedOverZ = false;
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

            _translationFinished->raise(new phi::translationEventArgs(_transform, _startLocalPos, _transform->getLocalPosition()));
        }
    }

    void translationControl::onMouseMove(mouseEventArgs* e)
    {
        if (!_transform)
            return;

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

        if (_clickedOverX || _clickedOverY || _clickedOverZ)
        {
            auto screenObjectPos = worldToScreen(_startPos);
            auto screenObjectDir = glm::normalize(worldToScreen(_startPos + dir) - screenObjectPos);
            auto mouseStartToEnd = vec2(e->x, e->y) - screenObjectPos;
            auto screenObjectEndPos = screenObjectPos + screenObjectDir * (dot(mouseStartToEnd, screenObjectDir) - glm::dot(_mouseStartPos - screenObjectPos, screenObjectDir));
            auto viewObjectEndPos = screenToViewZNear(screenObjectEndPos);

            auto worldToView = [&](const vec3 vec)
            {
                auto v = vec4(vec.x, vec.y, vec.z, 1.0f);
                v = _camera->getViewMatrix() * v;
                return vec3(v.x, v.y, v.z);
            };

            auto viewObjectStartPos = worldToView(_startPos);
            auto viewObjectStartDirPos = worldToView(_startPos + dir);

            vec3 p1, p2;
            float t1, t2;
            if (lineLineIntersect(vec3(), viewObjectEndPos, viewObjectStartPos, viewObjectStartDirPos, &p1, &p2, &t1, &t2))
            {
                //auto world4 = inverse(_camera->getViewMatrix()) * vec4(p1.x, p1.y, p1.z, 1.0f);
                //auto world = vec3(world4.x, world4.y, world4.z);
                //auto model = (_object->getParent() == nullptr ? mat4() : _object->getParent()->getModelMatrix());
                //auto localWorld = multMat3(world, inverse(model));
                auto localWorld = _startPos + dir * t2;

                auto currentPos = _transform->getLocalPosition();
                _transform->setLocalPosition(localWorld);

                auto args = new translationEventArgs(_transform, currentPos, localWorld);
                _translating->raise(args);

                if (args->cancel)
                    _transform->setLocalPosition(currentPos);

                safeDelete(args);
            }
        }
    }

    void translationControl::onMouseLeave(mouseEventArgs* e)
    {
        if (!_transform)
            return;

        //colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
        //colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
        //colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);
    }

    void translationControl::attachTo(transform* transform)
    {
        _transform = transform;

        updateModelMatrix();
        updateAabbs();
    }

    void translationControl::renderArrow(geometry* geometry, color color, mat4 modelMatrix)
    {
        auto mvp = _camera->getProjectionMatrix() * _camera->getViewMatrix() * _modelMatrix * modelMatrix;

        _shader->bind();
        //_shader->setUniform("mvp", mvp);
        //_shader->setUniform("color", color);
        //geometryRenderer::render(geometry);
        _shader->unbind();
    }

    void translationControl::onRender()
    {
        if (!_transform)
            return;

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        updateModelMatrix();
        updateAabbs();
        renderArrow(_arrowGeometry, _xColor, _xModelMatrix);
        renderArrow(_arrowGeometry, _yColor, _yModelMatrix);
        renderArrow(_arrowGeometry, _zColor, _zModelMatrix);

        //Draw AABB:
        //auto min = _object->getAabb()->getMin();
        //auto max = _object->getAabb()->getMax();
        //auto lbb = vec3(min.x, min.y, min.z);
        //auto lbf = vec3(min.x, min.y, max.z);
        //auto ltf = vec3(min.x, max.y, max.z);
        //auto ltb = vec3(min.x, max.y, min.z);
        //auto rbb = vec3(max.x, min.y, min.z);
        //auto rbf = vec3(max.x, min.y, max.z);
        //auto rtf = vec3(max.x, max.y, max.z);
        //auto rtb = vec3(max.x, max.y, min.z);

        //auto mvp = _camera->getPerspProjMatrix() * _camera->getViewMatrix();
        //_shader->bind();
        //_shader->setUniform("mvp", mvp);
        //_shader->setUniform("color", color::orange);
        //auto pos = std::vector<vec3>();
        //pos.push_back(lbb);
        //pos.push_back(lbf);
        //pos.push_back(ltf);
        //pos.push_back(ltb);
        //pos.push_back(rbb);
        //pos.push_back(rbf);
        //pos.push_back(rtf);
        //pos.push_back(rtb);
        //auto ind = std::vector<GLuint>();
        //ind.push_back(0);
        //ind.push_back(1);
        //ind.push_back(1);
        //ind.push_back(2);
        //ind.push_back(2);
        //ind.push_back(3);
        //ind.push_back(3);
        //ind.push_back(0);
        //ind.push_back(4);
        //ind.push_back(5);
        //ind.push_back(5);
        //ind.push_back(6);
        //ind.push_back(6);
        //ind.push_back(7);
        //ind.push_back(7);
        //ind.push_back(4);
        //ind.push_back(0);
        //ind.push_back(4);
        //ind.push_back(1);
        //ind.push_back(5);
        //ind.push_back(2);
        //ind.push_back(6);
        //ind.push_back(3);
        //ind.push_back(7);

        //auto a = linegeometry::create("oi", pos, ind);
        //a->bind();
        //a->render();
        //a->unbind();

        //DELETE(a);
        //_shader->unbind();
    }
}