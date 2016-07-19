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
        vec3 _halfSizes;
        vec3 _position;
        obb _obb;
        eventToken _transformChangedEventToken;
        mat4 _localModelMatrix;
        mat4 _worldModelMatrix;

    private:
        transform* getTransform();
        void transformChanged(transform * const sender);

    protected:
        void onNodeChanged(node* previousValue) override;

    public:
        CORE_API boxCollider(std::string name, vec3 position, vec3 halfSizes);
        CORE_API boxCollider(const boxCollider& original);
        CORE_API ~boxCollider();
        CORE_API component* clone() const override;

        vec3 getHalfSizes() const { return _halfSizes; }
        vec3 getPosition() const { return _position; }
        obb getObb() const { return _obb; }
        mat4 getWorldModelMatrix() const { return _worldModelMatrix; }
        mat4 getLocalModelMatrix() const { return _localModelMatrix; }


        CORE_API static componentType getComponentType() { return componentType::BOX_COLLIDER; }
    };
}