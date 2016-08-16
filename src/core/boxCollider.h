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
        obb _obb;
        eventToken _transformChangedEventToken;
        mat4 _modelMatrix;

    private:
        transform* getTransform();
        void updateModelMatrix();
        void nodeTransformChanged(transform * const sender);

    protected:
        void onNodeChanged(node* previousValue) override;

    public:
        CORE_API boxCollider(vec3 position, vec3 halfSizes);
        CORE_API boxCollider(const boxCollider& original);
        CORE_API ~boxCollider();
        CORE_API component* clone() const override;

        vec3 getSize() const { return _size; }
        vec3 getCenter() const { return _center; }
        obb getObb() const { return _obb; }
        mat4 getModelMatrix() const { return _modelMatrix; }

        CORE_API static componentType getComponentType() { return componentType::BOX_COLLIDER; }
    };
}