#include <phi\core\object3D.h>

namespace phi
{
    object3D::object3D(std::string name, objectType type) :
        _name(name),
        _type(type),
        _position(vec3()),
        _right(vec3(1.0f, 0.0f, 0.0f)),
        _up(vec3(0.0f, 1.0f, 0.0f)),
        _direction(vec3(0.0f, 0.0f, 1.0f)),
        _orientation(quat()),
        _changedEvent(new eventHandler<object3DEventArgs>()),
        _size(sizef(1.0f, 1.0f, 1.0f)),
        _aabb(nullptr),
        _parent(nullptr)
    {
        setChanged();
    }

    object3D::~object3D()
    {
        safeDelete(_aabb);

        // TODO: Do I need to delete those, or should the scene?
        // auto _childrenCount = _children.size();
        // for (unsigned int i = 0; i < _childrenCount; i++)
        //     DELETE(_children[i]);
    }

    void object3D::setChanged()
    {
        if(_changed)
            return;

        _changed = true;

        //auto childrenCount = _children.size();
        //for (unsigned int i = 0; i < childrenCount; i++)
        //    _children[i]->setChanged();
        //TODO: are those the same ?
        for(auto& child : _children)
            child->setChanged();

        onChanged();
        if(_changedEvent->isBound())
            _changedEvent->invoke(object3DEventArgs(this));
    }

    void object3D::setLocalPosition(vec3 value)
    {
        _position = value;
        setChanged();
        onPositionChanged();
    }

    void object3D::setOrientation(quat value)
    {
        _orientation = value;
        setChanged();
        onDirectionChanged();
    }

    void object3D::setDirection(vec3 direction)
    {
        _orientation = mathUtils::rotationBetweenVectors(_direction, direction) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::setSize(sizef value)
    {
        _size = value;
        setChanged();
    }

    vec3 object3D::getPosition()
    {
        auto v = getModelMatrix() * vec4(0.0f, 0.0f, 0.0f, 1.0);
        return vec3(v.x, v.y, v.z);
    }

    mat4 object3D::getRotationMatrix()
    {
        return toMat4(_orientation);
    }

    mat4 object3D::getTranslationMatrix()
    {
        return mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _position.x, _position.y, _position.z, 1.0f);
    }

    mat4 object3D::getScaleMatrix()
    {
        return mat4(
            _size.w, 0.0f, 0.0f, 0.0f,
            0.0f, _size.h, 0.0f, 0.0f,
            0.0f, 0.0f, _size.d, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4 object3D::getModelMatrix()
    {
        if(_parent == nullptr)
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
        auto localRotation = getRotationMatrix();
        auto localScale = getScaleMatrix();
        auto localTranslation = getTranslationMatrix();

        auto rotation = localRotation;
        auto obj = this->getParent();

        while(obj != nullptr)
        {
            rotation = obj->getRotationMatrix() * rotation;
            obj = obj->getParent();
        }

        _direction = mathUtils::multiply(rotation, vec3(0.0f, 0.0f, 1.0f));
        _right = mathUtils::multiply(rotation, vec3(1.0f, 0.0f, 0.0f));
        _up = mathUtils::multiply(rotation, vec3(0.0f, 1.0f, 0.0f));

        _modelMatrix = localTranslation  * localRotation * localScale;

        auto normalMatrix = mat3(
            _modelMatrix[0][0], _modelMatrix[0][1], _modelMatrix[0][2],
            _modelMatrix[1][0], _modelMatrix[1][1], _modelMatrix[1][2],
            _modelMatrix[2][0], _modelMatrix[2][1], _modelMatrix[2][2]);

        for(auto child : _children)
            child->updateLocalModelMatrix();

        //TODO: are those the same ?
        //auto childrenCount = _children.size();
        // for (unsigned int i = 0; i < childrenCount; i++)
        //     _children[i]->updateLocalModelMatrix();
    }

    void object3D::update()
    {
        if(_changed)
        {
            updateLocalModelMatrix();
            _changed = false;
        }
    }

    void object3D::translate(vec3 translation)
    {
        _position += translation;
        setChanged();
    }

    void object3D::rotate(float angle, vec3 axis)
    {
        _orientation = angleAxis(angle, axis) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::pitch(float angle)
    {
        _orientation = angleAxis(angle, vec3(1.0f, 0.0f, 0.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::yaw(float angle)
    {
        _orientation = angleAxis(angle, vec3(0.0f, 1.0f, 0.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }

    void object3D::roll(float angle)
    {
        _orientation = angleAxis(angle, vec3(0.0f, 0.0f, 1.0f)) * _orientation;
        setChanged();
        onDirectionChanged();
    }
}