#include <precompiled.h>
#include "layoutTransform.h"

namespace phi
{
    layoutTransform::layoutTransform() :
        transform()
    {
    }

    void layoutTransform::updateData()
    {
        auto localRotation = getLocalRotationMatrix();
        auto localTranslation = getLocalTranslationMatrix();
        _localModelMatrix = localTranslation  * localRotation;
        _changed = false;

        if (_parent)
        {
            auto parentOrientation = _parent->getOrientation();
            _position = _parent->getPosition() + parentOrientation * _localPosition;
            _orientation = parentOrientation * _localOrientation;
        }
        else
        {
            _position = _localPosition;
            _orientation = _localOrientation;
        }

        _size = _localSize;

        _right = _orientation * vec3(1.0f, 0.0f, 0.0f);
        _up = _orientation * vec3(0.0f, 1.0f, 0.0f);
        _direction = _orientation *  vec3(0.0f, 0.0f, 1.0f);
    }
}