#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <core\node.h>
#include <core\plane.h>
#include <animation\transformAnimation.h>
#include <ui\planeGrid.h>

namespace phi
{
    struct translationPlane
    {
    public:
        plane plane;
        boxCollider* collider;
        node* planeGridNode;
        transformAnimation* planeGridAnimation;

    public:
        translationPlane(phi::plane plane) :
            plane(plane),
            collider(nullptr),
            planeGridNode(nullptr),
            planeGridAnimation(nullptr)
        {
        }

        void showGrid()
        {
            planeGridNode->getComponent<planeGrid>()->show();
        }

        void hideGrid()
        {
            planeGridNode->getComponent<planeGrid>()->hide();
        }
    };
}