#pragma once

#include <phi.h>
#include "freeMouseDrag.h"

namespace demon
{
    class planesMouseDrag :
        public freeMouseDrag
    {
    private:
        struct touchingCollisionResult
        {
            touchingCollisionResult() :
                foundValidCollision(false),
                collider(nullptr),
                normal(),
                collisionsCount(0u)
            {
            }

            bool foundValidCollision;
            phi::boxCollider* collider;
            phi::vec3 normal;
            phi::uint collisionsCount;
        };

        phi::vec2 _lastMousePosition;
        phi::boxCollider* _currentTouchingCollider;
        phi::plane _originalPlane;
        phi::vec3 _planeOffsetToObject;

    private:
        touchingCollisionResult findTouchingCollision(phi::vec3 dragDirection);

    public:
        planesMouseDrag(phi::scene* scene);

        virtual void startDrag(int mouseX, int mouseY) override;
        virtual void drag(int mouseX, int mouseY) override;
        virtual void endDrag() override;
    };
}