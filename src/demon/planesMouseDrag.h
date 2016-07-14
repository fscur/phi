#pragma once

#include <phi.h>
#include "obbMouseDrag.h"

namespace demon
{
    class planesMouseDrag :
        public obbMouseDrag
    {
    private:
        phi::vec2 _lastMousePosition;
        phi::boxCollider* _planeSource;
        phi::plane _originalPlane;
        phi::vec3 _planeOffsetToObject;

    public:
        planesMouseDrag(phi::scene* scene);

        virtual void startDrag(int mouseX, int mouseY) override;
        virtual void drag(int mouseX, int mouseY) override;
        virtual void endDrag() override;
    };
}