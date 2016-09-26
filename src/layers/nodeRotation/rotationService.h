#pragma once
#include <phi.h>

#include <core/node.h>
#include <core/boxCollider.h>
#include <physics/physicsWorld.h>
#include <application/commandsManager.h>
#include <context/layer.h>

#include "rotationPlane.h"

namespace phi
{
    class rotationService
    {
    public:
        rotationService(
            const vector<node*>* targetNodes,
            layer* layer,
            physicsWorld* physicsWorld);

        ~rotationService();

        void startRotation(ivec2 mousePosition);
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
        void createPlanes();
        rotationPlane * createAxisAlignedRotationPlane(ivec2 mousePosition);

        plane createPlaneFromAxis(const vec3 & axis);

        rotationPlane * createRotationPlane(const plane & plane);

        void changePlanes(rotationPlane * rotationPlane, const plane & offsetPlane);

        void enqueuePlaneForDeletion(rotationPlane * planeToRemove);

        void showRotationPlane();

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;

        bool _isRotating;
        ivec2 _lastMousePosition;
        vec3 _offsetPlaneOrigin;
        rotationPlane* _currentRotationPlane;
        plane _offsetPlane;
        vector<rotationPlane*> _planesToDelete;
    };
}
