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

    private:
        transform* getTransform();
        void transformChanged(transform * const sender);

    protected:
        void onNodeChanged(node* previousValue) override;

    public:
        CORE_API boxCollider(std::string name, vec3 position, vec3 halfSizes);
        CORE_API ~boxCollider();

        vec3 getHalfSizes() const { return _halfSizes; }
        vec3 getPosition() const { return _position; }
        obb getObb() const { return _obb; }

        CORE_API static componentType getComponentType() { return componentType::BOX_COLLIDER; }
    };
}