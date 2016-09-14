#pragma once
#include <phi.h>

#include <core/boxCollider.h>
#include <physics/physicsWorld.h>
#include <application/commandsManager.h>
#include <context/layer.h>

#include "collisionNodeTranslator.h"
#include "translationPlane.h"
#include "ghostNodeTranslator.h"

namespace phi
{
    class translationService
    {
    public:
        translationService(
            const vector<node*>* targetNodes,
            layer* layer,
            physicsWorld* physicsWorld);

        ~translationService();

        void startTranslation(ivec2 mousePosition);
        void translate(ivec2 mousePosition);
        void endTranslation();

        void update();

        void disableCollisions();
        void enableCollisions();

        void disablePlaneChanges();
        void enablePlaneChanges();

        bool isTranslating() const { return _isTranslating; }
        
    private:
        void resetTargetNodesColliders();

        void createPlanes();
        plane createPlaneFromAxis(const vec3& axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);
        translationPlane* createTranslationPlane(const plane& plane);
        void showTranslationPlane();

        void enqueuePlaneForDeletion(translationPlane* planeToRemove);
        void deletePlane(translationPlane* translationPlane);
        void deletePlaneIfNotVisible();
        
        void updatePlaneVisibility();

        bool canChangeTo(const plane& plane);
        bool isPlaneVisible(const plane& plane);
        float getExtinctionFactor(const vec3& normal);
        float getPlaneVisibility(const plane& plane);
        bool isNormalValidForCollision(const sweepCollision & touch, const vec3 & normal);
        
        bool tryChangingPlanes();
        bool tryChangeToPlanesFromCollisions();
        bool tryChangeToAttachedPlane();
        void changePlanes(translationPlane * translationPlane, const plane& offsetPlane);
        
        void translateTargetNodes(const vec3& targetPosition);
        void translatePlaneGrid(const vec3& targetPosition);

        void resetCurrentCollisions();
        void checkCollisionsBeforeTranslation();
        void checkCollisionsAferTranslation();
        vector<sweepCollision> findTouchingCollisions(const vec3& direction, float distance);
        vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;
        collisionNodeTranslator* _nodeTranslator;
        ghostNodeTranslator* _ghostTranslator;

        bool _isTranslating;
        bool _canChangePlanes;

        vec3 _offsetPlaneOrigin;
        plane _offsetPlane;
        translationPlane* _currentTranslationPlane;
        vector<translationPlane*> _planesToDelete;
        ivec2 _lastMousePosition;

        vector<sweepCollision>* _lastCollisions;
        vector<sweepCollision> _currentCollisions;

        vector<boxCollider*> _targetNodesColliders;
        vector<clippingPlane*> _clippingPlanes;
        vector<translationPlane*> _clippingTranslationPlanes;
    };
}
