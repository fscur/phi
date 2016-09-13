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
        vec3 getClosestAxisTo(vec3 direction);
        plane createPlaneFromAxis(vec3 axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);

        translationPlane* createTranslationPlane(plane plane);
        void addTranslationPlane(translationPlane * translationPlane);
        void enqueuePlaneForRemoval(translationPlane* planeToRemove);
        void removePlanesIf(std::function<bool(translationPlane*)> predicate);
        void removeInvalidPlanes();
        void deleteRemovedPlanes();
        void updateTranslationPlanesVisibility();

        bool isPlaneValidForAddition(plane plane);
        bool isPlaneVisible(plane plane);
        float getExtinctionFactor(vec3 normal);
        float getPlaneVisibility(plane plane);

        void addPlaneIfNeeded();

        bool isNormalValidForCollision(const sweepCollision & touch, const vec3 & normal);
        
        vector<sweepCollision> findTouchingCollisions(vec3 direction, float distance);
        vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);

        void addValidPlanesFromTouchCollisions();
        void changePlanes(translationPlane * translationPlane, plane* offsetPlane);
        bool changePlanesIfNeeded();
        bool changeToAttachedPlane(ivec2 mousePosition);
        void translateTargetNodes(vec3 endPosition);
        void translatePlaneGrid(vec3 endPosition);

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;
        collisionNodeTranslator* _nodeTranslator;
        ghostNodeTranslator* _ghostTranslator;

        vector<translationPlane*> _translationPlanes;
        vector<translationPlane*> _planesToRemove;

        unordered_map<translationPlane*, plane*> _offsetPlanes;

        bool _isTranslating;
        bool _canChangePlanes;
        plane* _lastVisiblePlane;
        vec3 _offsetPlaneOrigin;
        plane* _currentOffsetPlane;
        translationPlane* _currentTranslationPlane;

        ivec2 _lastMousePosition;

        vector<sweepCollision>* _lastTouchingCollisions;
        vector<sweepCollision> _currentCollisions;

        vector<boxCollider*> _targetNodesColliders;
        vector<clippingPlane*> _clippingPlanes;
        vector<translationPlane*> _clippingTranslationPlanes;
    };
}
