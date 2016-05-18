#pragma once

#include "scenesApi.h"

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
        boxCollider(std::string name, vec3 position, vec3 halfSizes);
        ~boxCollider();

        SCENES_API vec3 getHalfSizes() const { return _halfSizes; }
        SCENES_API vec3 getPosition() const { return _position; }
        SCENES_API obb getObb() const { return _obb; }

        static componentType getComponentType() { return componentType::BOX_COLLIDER; }
    };
}