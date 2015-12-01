#include <phi/core/object3D.h>

namespace phi
{
    object3D::object3D()
    {
        _position = glm::vec3(0.0f, 0.0f, 0.0f);
        _right = glm::vec3(1.0f, 0.0f, 0.0f);
        _up = glm::vec3(0.0f, 1.0f, 0.0f);
        _direction = glm::vec3(0.0f, 0.0f, 1.0f);
        _orientation = mathUtils::rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), _direction);
        _changedEvent = new eventHandler<object3DEventArgs>();

        _yaw = 0;
        _roll = 0;
        _pitch = 0;

        _size = size<float>(1.0f, 1.0f, 1.0f);
        setChanged();
        _aabb = nullptr;
        _parent = nullptr;
    }

    object3D::~object3D()
    {
        safeDelete(_aabb);

        // Do I need to delete those, or should the scene?
        //auto _childrenCount = _children.size();
        //for (unsigned int i = 0; i < _childrenCount; i++)
        //    DELETE(_children[i]);
    }

    void object3D::setChanged()
    {
        if (_changed)
            return;

        _changed = true;

        auto childrenCount = _children.size();
        for (unsigned int i = 0; i < childrenCount; i++)
            _children[i]->setChanged();

        onChanged();
        if (_changedEvent->isBound())
            _changedEvent->invoke(object3DEventArgs(this));
    }

    void object3D::setLocalPosition(glm::vec3 value)
    {
        _position = value;
        setChanged();
        onPositionChanged();
    }

    void object3D::setOrientation(glm::quat value)
    {
        _orientation = value;
        setChanged();
        onDirectionChanged();
    }

    void object3D::setDirection(glm::vec3 direction)
    {
        _orientation = mathUtils::rotationBetweenVectors(_direction, direction) * _orientation;
        setChanged();

        onDirectionChanged();
    }

    void object3D::setSize(size<float> value)
    {
        _size = value;
        setChanged();
    }

    glm::vec3 object3D::getPosition()
    {
        auto vec4 = getModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0);
        return glm::vec3(vec4.x, vec4.y, vec4.z);
    }

    glm::vec3 object3D::getLocalPosition()
    {
        return _position;
    }

    glm::vec3 object3D::getDirection()
    {
        return _direction;
    }

    glm::vec3 object3D::getRight()
    {
        return _right;
    }

    glm::vec3 object3D::getUp()
    {
        return _up;
    }

    glm::mat4 object3D::getRotationMatrix()
    {
        return glm::toMat4(_orientation);
    }

    glm::mat4 object3D::getTranslationMatrix()
    {
        return glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _position.x, _position.y, _position.z, 1.0f);
    }

    glm::mat4 object3D::getScaleMatrix()
    {
        return glm::mat4(
            _size.width, 0.0f, 0.0f, 0.0f,
            0.0f, _size.height, 0.0f, 0.0f,
            0.0f, 0.0f, _size.depth, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    glm::mat4 object3D::getModelMatrix()
    {
        if (_parent == nullptr)
            return _modelMatrix;
        else
            return _parent->getModelMatrix() * _modelMatrix;
    }

    void object3D::addChild(object3D* child)
    {
        _children.push_back(child);
        child->setParent(this);
    }

    void object3D::updateLocalModelMatrix()
    {
        glm::mat4 localRotation = getRotationMatrix();
        glm::mat4 localScale = getScaleMatrix();
        glm::mat4 localTranslation = getTranslationMatrix();

        auto rotation = localRotation;
        auto obj = this->getParent();
        while (obj != nullptr)
        {
            rotation = obj->getRotationMatrix() * rotation;
            obj = obj->getParent();
        }

        _direction = mathUtils::multiply(rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        _right = mathUtils::multiply(rotation, glm::vec3(1.0f, 0.0f, 0.0f));
        _up = mathUtils::multiply(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        _modelMatrix = localTranslation  * localRotation * localScale;

        glm::mat3 normalMatrix = glm::mat3(
            _modelMatrix[0][0], _modelMatrix[0][1], _modelMatrix[0][2],
            _modelMatrix[1][0], _modelMatrix[1][1], _modelMatrix[1][2],
            _modelMatrix[2][0], _modelMatrix[2][1], _modelMatrix[2][2]);

        auto childrenCount = _children.size();
        for (unsigned int i = 0; i < childrenCount; i++)
            _children[i]->updateLocalModelMatrix();
    }

    void object3D::update()
    {
        if (_changed)
        {
            updateLocalModelMatrix();
            _changed = false;
        }
    }

    void object3D::translate(glm::vec3 translation)
    {
        _position += translation;
        setChanged();
    }

    void object3D::rotate(float angle, glm::vec3 axis)
    {
        _orientation = glm::angleAxis(angle, axis) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::pitch(float angle)
    {
        _pitch += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::yaw(float angle)
    {
        _yaw += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::roll(float angle)
    {
        _roll += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }
}