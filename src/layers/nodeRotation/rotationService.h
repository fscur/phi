#pragma once
#include <phi.h>

#include <core/node.h>
#include <core/boxCollider.h>
#include <physics/physicsWorld.h>
#include <application/commandsManager.h>
#include <context/layer.h>
#include <layers/nodeTranslation/ghostNodeTranslator.h>

#include "collisionNodeRotator.h"
#include "rotationPlane.h"

namespace phi
{
    class rotationService
    {
    public:
        enum rotationUsageMode
        {
            ROTATE_AT_CENTROID,
            ROTATE_AT_MOUSE_POSITION,
            ROTATE_AT_INDIVIDUAL_ORIGINS
        };

    public:
        rotationService(
            const vector<node*>* targetNodes,
            layer* layer,
            physicsWorld* physicsWorld);

        ~rotationService();

        rotationUsageMode getUsageMode() { return _usageMode; }
        void setUsageMode(rotationUsageMode value);

        void startRotation(ivec2 mousePosition, node* clickedNode);
        void rotate(ivec2 mousePosition);
        void endRotation();

        void update();
        void deletePlane(rotationPlane * rotationPlane);
        void updatePlaneVisibility(rotationPlane * rotationPlane);
        float getPlaneVisibility(const plane & plane);
        bool isPlaneVisible(const plane & plane);
        float getExtinctionFactor(const vec3 & normal);
        void deletePlaneIfNotVisible();
        void disableCollisions();
        void enableCollisions();
        bool isRotating() const { return _isRotating; }

    private:
        void createPlane();
        rotationPlane * createAxisAlignedRotationPlane(ivec2 mousePosition);
        vec3 getTargetNodesCentroid();
        plane createPlaneFromAxis(const vec3 & axis);
        rotationPlane * createRotationPlane(const plane & plane);
        void changePlane(rotationPlane * rotationPlane);
        void enqueuePlaneForDeletion(rotationPlane * planeToRemove);
        void showRotationPlane();
        void addMouseMovementDeltaAngle(ivec2 mousePosition);
        void rotateTargetNodes();
        void rotateGhostNodes();
        void updateRotationPlaneFilledAngle();

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;
        collisionNodeRotator* _nodeRotator;
        ghostNodeTranslator* _ghostTranslator;

        bool _isRotating;
        node* _clickedNode;
        ivec2 _lastMousePosition;
        vec3 _rotationLastPosition;
        rotationPlane* _currentRotationPlane;
        vector<rotationPlane*> _planesToDelete;
        plane _currentPlane;
        float _lastAngle;
        float _lastCollidedAngle;
        rotationUsageMode _usageMode;
    };
}
