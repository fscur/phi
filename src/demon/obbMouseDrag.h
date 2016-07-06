#pragma once

#include <phi.h>

#include "mouseDrag.h"

namespace demon
{
    class obbMouseDrag :
        public mouseDrag
    {
    private:
        bool _dragging;
        phi::boxCollider* _dragCollider;
        phi::planeGridRenderPass* _planeGridPass;
        phi::image* _gridImage;

    public:
        obbMouseDrag(phi::scene* scene);
        ~obbMouseDrag() override;

        virtual void startDrag(int mouseX, int mouseY) override;
        virtual void drag(int mouseX, int mouseY) override;
        virtual void endDrag() override;
    };
}