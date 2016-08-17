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
        boxCollider* sourceCollider;
        node* planeGridNode;
        transformAnimation* transformAnimation;
        floatAnimation* fadeInAnimation;

    public:
        translationPlane(phi::plane plane) :
            plane(plane),
            collider(nullptr),
            planeGridNode(nullptr),
            transformAnimation(nullptr),
            fadeInAnimation(nullptr)
        {
        }

        void showGrid()
        {
            planeGridNode->getComponent<planeGrid>()->show();
            fadeInAnimation->start(0.0f, 1.0f, 0.2);
        }

        void hideGrid()
        {
            planeGridNode->getComponent<planeGrid>()->hide();
        }
    };
}