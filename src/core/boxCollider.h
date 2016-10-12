#pragma once

#include "coreApi.h"

#include <core\component.h>
#include <core\obb.h>

namespace phi
{
    class boxCollider :
        public component
    {
    private:
        vec3 _center;
        vec3 _size;
        obb _localObb;
        obb _obb;
        eventToken _transformChangedEventToken;
        mat4 _modelMatrix;
        bool _isEnabled;
        eventHandler<boxCollider*> _isEnabledChanged;

    private:
        transform* getTransform();
        void updateModelMatrix();
        void nodeTransformChanged(transform * const sender);
        void raiseIsEnabledChanged();

    protected:
        void onNodeChanged(node* previousValue) override;

    public:
        CORE_API boxCollider(vec3 position, vec3 size);
        CORE_API boxCollider(const boxCollider& original);
        CORE_API ~boxCollider();
        CORE_API component* clone() const override;

        CORE_API void enable();
        CORE_API void disable();

        vec3 getSize() const { return _size; }
        vec3 getCenter() const { return _center; }
        obb getLocalObb() const { return _localObb; }
        obb getObb() const { return _obb; }
        mat4 getModelMatrix() const { return _modelMatrix; }
        bool getIsEnabled() const { return _isEnabled; }
        eventHandler<boxCollider*>* getIsEnabledChanged() { return &_isEnabledChanged; }

        CORE_API static componentType getComponentType() { return componentType::BOX_COLLIDER; }
    };
}