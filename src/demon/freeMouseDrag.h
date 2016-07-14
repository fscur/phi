#pragma once

#include <phi.h>

#include "mouseDrag.h"

namespace demon
{
    class freeMouseDrag :
        public mouseDrag
    {
    protected:
        bool _dragging;
        phi::boxCollider* _dragCollider;
        phi::planeGridRenderPass* _planeGridPass;
        phi::image* _gridImage;

        void initializeDragObjectData(phi::node* node);
        void setPlane(phi::plane plane);
        void showPlaneGrid(phi::vec3 position, phi::color color);

    public:
        freeMouseDrag(phi::scene* scene);
        ~freeMouseDrag() override;

        virtual void startDrag(int mouseX, int mouseY) override;
        virtual void drag(int mouseX, int mouseY) override;
        virtual void endDrag() override;
    };
}