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
        plane createPlaneFromAxis(vec3 axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);

        translationPlane* createTranslationPlane(plane plane);
        void showTranslationPlane();
        void enqueuePlaneForDeletion(translationPlane* planeToRemove);
        
        void deletePlane(translationPlane* translationPlane);
        void deletePlaneIfNotVisible();
        void updatePlaneVisibility();

        bool canChangeTo(plane plane);
        bool isPlaneVisible(plane plane);
        float getExtinctionFactor(vec3 normal);
        float getPlaneVisibility(plane plane);

        void createPlaneIfNeeded();

        bool isNormalValidForCollision(const sweepCollision & touch, const vec3 & normal);
        
        vector<sweepCollision> findTouchingCollisions(vec3 direction, float distance);
        vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);

        bool tryChangingPlanes(ivec2 mousePosition);
        bool tryChangeToPlanesFromCollisions();
        bool tryChangeToAttachedPlane(ivec2 mousePosition);

        void changePlanes(translationPlane * translationPlane, plane offsetPlane);
        
        void translateTargetNodes(vec3 endPosition);
        void translatePlaneGrid(vec3 endPosition);

        void checkCollisionsBeforeTranslation();
        void checkCollisionsAferTranslation();

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

        vector<sweepCollision>* _lastTouchingCollisions;
        vector<sweepCollision> _currentCollisions;

        vector<boxCollider*> _targetNodesColliders;
        vector<clippingPlane*> _clippingPlanes;
        vector<translationPlane*> _clippingTranslationPlanes;
    };
}
