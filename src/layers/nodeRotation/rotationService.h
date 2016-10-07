#pragma once
#include <phi.h>

#include <core/node.h>
#include <core/boxCollider.h>
#include <physics/physicsWorld.h>
#include <application/commandsManager.h>
#include <context/layer.h>
#include <layers/nodeTranslation/collisionNodeTranslator.h>

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
        void setUsageMode(rotationUsageMode value) { _usageMode = value; }

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

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;
        collisionNodeTranslator* _nodeTranslator;

        bool _isRotating;
        node* _clickedNode;
        ivec2 _lastMousePosition;
        vec3 _rotationStartPosition;
        rotationPlane* _currentRotationPlane;
        vector<rotationPlane*> _planesToDelete;
        plane _currentPlane;
        float _lastAngle;
        rotationUsageMode _usageMode;
        unordered_map<node*, vector<boxCollider*>> _targetNodesColliders;
    };
}
