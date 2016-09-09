#pragma once
#include <phi.h>

#include <context/layer.h>

#include <core/boxCollider.h>

#include <physics/physicsWorld.h>

#include <application/commandsManager.h>

#include "translationPlane.h"
#include "collisionNodeTranslator.h"
#include "ghostNodeTranslator.h"

namespace phi
{
    class translationService
    {
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

    private:
        vec3 getClosestAxisTo(vec3 direction);
        plane createPlaneFromAxis(vec3 axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);

        translationPlane* createTranslationPlane(plane plane);
        void addAxisAlignedTranslationPlane();
        void addTranslationPlane(translationPlane * translationPlane);
        void enqueuePlaneForRemoval(translationPlane* planeToRemove);
        void removePlanesIf(std::function<bool(translationPlane*)> predicate);
        void removeInvalidPlanes();
        void deleteRemovedPlanes();
        void updateTranslationPlanesVisibility();

        bool isPlaneValidForAddition(plane plane);
        bool isPlaneVisible(plane plane, float planeExtinctionFactor);
        float getExtinctionFactor(vec3 normal);
        float getPlaneVisibility(plane plane);

        void addPlaneIfNeeded();

        bool isNormalValidForCollision(const sweepCollision & touch, const vec3 & normal);

        void addValidPlanesFromTouchCollisions();
        void changePlanes(translationPlane * translationPlane, plane* offsetPlane);
        void changePlanesIfNeeded();
        plane getClosestPlaneToPosition(vector<plane>& planes, vec3 position);
        void changeToAttachedPlane(ivec2 mousePosition);
        void translateTargetNodes(vec3 endPosition);
        void translatePlaneGrid(vec3 endPosition);

    public:
        translationService(
            const vector<node*>* targetNodes, 
            layer* layer, 
            physicsWorld* physicsWorld);

        ~translationService();

        plane getClosestPlaneToDirection(vector<plane>& planes, vec3 direction);

        bool isTranslating() const { return _isTranslating; }
        size_t getTargetNodesCount() const { return _targetNodes->size(); }

        void startTranslation(ivec2 mousePosition);
        void translate(ivec2 mousePosition);
        void endTranslation();
        vector<sweepCollision> findTouchingCollisions(vec3 direction, float distance);
        vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);
        void checkCollisions();
        void cancelTranslation();
        void update();

        void disableCollisions();
        void enableCollisions();

        void disablePlaneChanges();
        void enablePlaneChanges();

        //vector<sweepCollision> findTouchingCollisions();
        //vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);
        //translationPlane* createTranslationPlane(plane plane, boxCollider* colidee, boxCollider* collider, clippingDistance::clippingDistance clippingDistance);
        /*void createClippingPlanes(translationPlane* clippingTranslationPlane, clippingDistance::clippingDistance clippingDistance);
        void removeClippingPlanes(translationPlane* planeToRemove);
        void addPlanesIfNeeded(vector<sweepCollision> touchs);*/
        /*bool existsTranslationPlaneWithNormal(vec3 normal);
        bool isDraggingObjectIntersectingAnyObject();*/
        
    };
}
