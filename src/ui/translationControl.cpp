#include "phi/ui/translationControl.h"

#include "phi/rendering/shaderManager.h"
#include "phi/rendering/meshRenderer.h"
#include "phi/rendering/lineMesh.h"

#include "phi/ui/colorAnimator.h"

#include <glm/gtx/constants.hpp>

namespace phi
{
    translationControl::translationControl(size<GLuint> viewportSize) :
        control(viewportSize)
    {
        _arrowMesh = createArrowMesh();
        _shader = shaderManager::get()->getShader("UI_MESH");
        _object = nullptr;
        _xAabb = new aabb(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        _yAabb = new aabb(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        _zAabb = new aabb(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        _xColor = color::fromRGBA(1.0f, 0.0f, 0.0f, 0.5f);
        _yColor = color::fromRGBA(0.0f, 1.0f, 0.0f, 0.5f);
        _zColor = color::fromRGBA(0.0f, 0.0f, 1.0f, 0.5f);
        _xModelMatrix = glm::rotate(glm::pi<float>() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        _yModelMatrix = glm::rotate(glm::pi<float>() * -0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        _zModelMatrix = glm::mat4();
        _clickedOverX = _clickedOverY = _clickedOverZ = false;
        _translationFinished = new eventHandler<translationEventArgs>();
    }

    mesh* translationControl::createArrowMesh()
    {
        std::vector<vertex> vertices;
        std::vector<GLuint> indices;

        auto sides = 10;
        auto pr = 0.05f;
        auto sr = 0.02f;
        auto ap = (glm::pi<float>() * 2.0f) / (float)sides;

        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.5f)); // 0

        for (auto i = 0; i < sides; i++)
            vertices.push_back(glm::vec3(glm::cos(ap * i) * pr, glm::sin(ap * i) * pr, 0.35f));

        for (auto i = 0; i < sides; i++)
            vertices.push_back(glm::vec3(glm::cos(ap * i) * sr, glm::sin(ap * i) * sr, 0.35f));

        for (auto i = 0; i < sides; i++)
            vertices.push_back(glm::vec3(glm::cos(ap * i) * sr, glm::sin(ap * i) * sr, 0.0f));

        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

        for (auto i = 1; i < sides + 1; i++)
        {
            indices.push_back(i);
            indices.push_back(i % sides + 1);
            indices.push_back(0);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices.push_back(i);
            indices.push_back(i + sides);
            indices.push_back(i % sides + 1);

            indices.push_back(i % sides + 1);
            indices.push_back(i + sides);
            indices.push_back(i % sides + sides + 1);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices.push_back(i + sides);
            indices.push_back(i + sides + sides);
            indices.push_back(i % sides + 1 + sides);

            indices.push_back(i % sides + 1 + sides);
            indices.push_back(i + sides + sides);
            indices.push_back(i % sides + sides + 1 + sides);
        }

        for (auto i = 1; i < sides + 1; i++)
        {
            indices.push_back(i + sides * 2);
            indices.push_back(sides * 3 + 1);
            indices.push_back(i % sides + 1 + sides * 2);
        }

        mesh::calcTangents(vertices, indices);

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

        return mesh::create("arrow", vertices, indices);
    }

    void translationControl::updateModelMatrix()
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

    void translationControl::updateAabbs()
    {
        if (!_object)
            return;

        auto min = glm::vec4(-0.80901699437494742410229341718282f * 0.05f, -0.95105651629515357211643933337938f * 0.05f, 0.0f, 1.0f);
        auto max = glm::vec4(1.0f * 0.05f, 0.95105651629515357211643933337938f * 0.05f, 0.5f, 1.0f);

        auto createAabb = [&] (const glm::mat4 modelMatrix)
        {
            auto newMin = _modelMatrix * modelMatrix * min;
            auto newMax = _modelMatrix * modelMatrix * max;
            return new aabb(glm::vec3(newMin.x, newMin.y, newMin.z), glm::vec3(newMax.x, newMax.y, newMax.z));
        };

        _xAabb = createAabb(_xModelMatrix);
        _yAabb = createAabb(_yModelMatrix);
        _zAabb = createAabb(_zModelMatrix);
    }

    ray translationControl::castRay(glm::vec2 screenCoords, size<unsigned int> screenSize)
    {
        float w = (float)screenSize.width;
        float h = (float)screenSize.height;
        float x = (2 * screenCoords.x)/w - 1.0f; 
        float y = 1.0f - (2 * screenCoords.y)/h;

        glm::mat4 invPersp = glm::inverse(_camera->getPerspProjMatrix());
        glm::mat4 invView = glm::inverse(_camera->getViewMatrix());

        glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 ray_eye = invPersp * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
        glm::vec3 ray_world = glm::vec3(invView * ray_eye);
        ray_world = glm::normalize(ray_world);

        return ray(_camera->getPosition(), ray_world);
    }

    bool translationControl::isPointInside(int x, int y)
    {
        if (!_object)
            return false;

        auto ray = castRay(glm::vec2(x, y), _viewportSize);
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

    glm::vec3 translationControl::screenToViewZNear(glm::vec2 mousePos)
    {
        float zNear = _camera->getFrustum()->getZNear();
        float aspect = _camera->getFrustum()->getAspect();
        float fov = _camera->getFrustum()->getFov();

        float tg = glm::tan(fov * 0.5f) * zNear;

        float h = _viewportSize.height;
        float w = _viewportSize.width;

        float hh = h * 0.5f;
        float hw = w * 0.5f;

        float ys = mousePos.y - hh;
        float yp = ys/hh;
        float y = -(yp * tg);

        float xs = mousePos.x - hw;
        float xp = xs/hw;
        float x = xp * tg * aspect;

        return glm::vec3(x, y, -zNear);
    }

    glm::vec2 translationControl::worldToScreen(glm::vec3 worldPos)
    {
        auto vp = _camera->getPerspProjMatrix() * _camera->getViewMatrix();
        auto vw = _viewportSize.width * 0.5f;
        auto vh = _viewportSize.height * 0.5f;
        auto v = glm::vec2(vw, vh);
        auto pos = glm::vec4(worldPos, 1.0);
        pos = vp * pos;
        pos = pos / pos.w;
        return glm::vec2(pos.x, -pos.y) * v + v;
    }

    int translationControl::lineLineIntersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 *pa, glm::vec3 *pb, float *mua, float *mub)
    {
        glm::vec3 p13,p43,p21;
        float d1343,d4321,d1321,d4343,d2121;
        float numer,denom;
        float epsilon = glm::epsilon<float>();

        p13.x = p1.x - p3.x;
        p13.y = p1.y - p3.y;
        p13.z = p1.z - p3.z;
        p43.x = p4.x - p3.x;
        p43.y = p4.y - p3.y;
        p43.z = p4.z - p3.z;
        if (glm::abs(p43.x) < epsilon && glm::abs(p43.y) < epsilon && glm::abs(p43.z) < epsilon)
            return false;
        p21.x = p2.x - p1.x;
        p21.y = p2.y - p1.y;
        p21.z = p2.z - p1.z;
        if (glm::abs(p21.x) < epsilon && glm::abs(p21.y) < epsilon && glm::abs(p21.z) < epsilon)
            return false;

        d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
        d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
        d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
        d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
        d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

        denom = d2121 * d4343 - d4321 * d4321;
        if (glm::abs(denom) < epsilon)
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
        if (!_object)
            return;

        if (e->leftButtonPressed && (_mouseOverX || _mouseOverY || _mouseOverZ))
        {
            auto multMat3 = [] (const glm::vec3 vec, const glm::mat4 mat)
            {
                auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
                vec4 = mat * vec4;
                return glm::vec3(vec4.x, vec4.y, vec4.z);
            };

            _clickedOverX = _mouseOverX;
            _clickedOverY = _mouseOverY;
            _clickedOverZ = _mouseOverZ;
            _startPos = _object->getPosition();
            _startLocalPos = _object->getLocalPosition();
            _mouseStartPos = glm::vec2(e->x, e->y);
            e->handled = true;
        }
    }

    void translationControl::onMouseUp(mouseEventArgs* e)
    {
        if (!_object)
            return;

        if (e->leftButtonPressed && (_clickedOverX || _clickedOverY || _clickedOverZ))
        {
            _clickedOverX = _clickedOverY = _clickedOverZ = false;
            colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
            colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);

            if (_translationFinished->isBound())
                _translationFinished->invoke(phi::translationEventArgs(_object, _startLocalPos, _object->getLocalPosition()));
        }
    }

    void translationControl::onMouseMove(mouseEventArgs* e)
    {
        if (!_object)
            return;

        auto multMat3 = [] (const glm::vec3 vec, const glm::mat4 mat)
        {
            auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
            vec4 = mat * vec4;
            return glm::vec3(vec4.x, vec4.y, vec4.z);
        };

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

        if (_clickedOverX || _clickedOverY || _clickedOverZ)
        {
            auto screenObjectPos = worldToScreen(_startPos);
            auto screenObjectDir = glm::normalize(worldToScreen(_startPos + dir) - screenObjectPos);
            auto mouseStartToEnd = glm::vec2(e->x, e->y) - screenObjectPos;
            auto screenObjectEndPos = screenObjectPos + screenObjectDir * (glm::dot(mouseStartToEnd, screenObjectDir) - glm::dot(_mouseStartPos - screenObjectPos, screenObjectDir));
            auto viewObjectEndPos = screenToViewZNear(screenObjectEndPos);

            auto worldToView = [&] (const glm::vec3 vec)
            {
                auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
                vec4 = _camera->getViewMatrix() * vec4;
                return glm::vec3(vec4.x, vec4.y, vec4.z);
            };

            auto viewObjectStartPos = worldToView(_startPos);
            auto viewObjectStartDirPos = worldToView(_startPos + dir);

            glm::vec3 p1, p2;
            float t1, t2;
            if (lineLineIntersect(glm::vec3(), viewObjectEndPos, viewObjectStartPos, viewObjectStartDirPos, &p1, &p2, &t1, &t2))
            {
                auto world4 = glm::inverse(_camera->getViewMatrix()) * glm::vec4(p1.x, p1.y, p1.z, 1.0f);
                auto world = glm::vec3(world4.x, world4.y, world4.z);

                auto model = (_object->getParent() == nullptr ? glm::mat4() : _object->getParent()->getModelMatrix());
                auto localWorld = multMat3(world, glm::inverse(model));
                _object->setLocalPosition(localWorld);
            }
        }
    }

    void translationControl::onMouseLeave(mouseEventArgs* e)
    {
        if (!_object)
            return;

        //colorAnimator::animateColor(&_xColor, color(1.0f, 0.0f, 0.0f, 0.5f), 300);
        //colorAnimator::animateColor(&_yColor, color(0.0f, 1.0f, 0.0f, 0.5f), 300);
        //colorAnimator::animateColor(&_zColor, color(0.0f, 0.0f, 1.0f, 0.5f), 300);
    }

    void translationControl::attachTo(object3D* object)
    {
        _object = object;

        updateModelMatrix();
        updateAabbs();
    }

    void translationControl::renderArrow(mesh* mesh, color color, glm::mat4 modelMatrix)
    {
        auto mvp = _camera->getPerspProjMatrix() * _camera->getViewMatrix() * _modelMatrix * modelMatrix;

        _shader->bind();
        _shader->setUniform("mvp", mvp);
        _shader->setUniform("color", color);
        meshRenderer::render(mesh);
        _shader->unbind();
    }

    void translationControl::onRender()
    {
        if (!_object)
            return;

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        updateModelMatrix();
        updateAabbs();
        renderArrow(_arrowMesh, _xColor, _xModelMatrix);
        renderArrow(_arrowMesh, _yColor, _yModelMatrix);
        renderArrow(_arrowMesh, _zColor, _zModelMatrix);

        //Draw AABB:
        //auto min = _object->getAabb()->getMin();
        //auto max = _object->getAabb()->getMax();
        //auto lbb = glm::vec3(min.x, min.y, min.z);
        //auto lbf = glm::vec3(min.x, min.y, max.z);
        //auto ltf = glm::vec3(min.x, max.y, max.z);
        //auto ltb = glm::vec3(min.x, max.y, min.z);
        //auto rbb = glm::vec3(max.x, min.y, min.z);
        //auto rbf = glm::vec3(max.x, min.y, max.z);
        //auto rtf = glm::vec3(max.x, max.y, max.z);
        //auto rtb = glm::vec3(max.x, max.y, min.z);

        //auto mvp = _camera->getPerspProjMatrix() * _camera->getViewMatrix();
        //_shader->bind();
        //_shader->setUniform("mvp", mvp);
        //_shader->setUniform("color", color::orange);
        //auto pos = std::vector<glm::vec3>();
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

        //auto a = lineMesh::create("oi", pos, ind);
        //a->bind();
        //a->render();
        //a->unbind();

        //DELETE(a);
        //_shader->unbind();
    }
}