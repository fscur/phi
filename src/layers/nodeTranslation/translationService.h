#pragma once
#include <phi.h>
#include "translationPlane.h"
#include "collisionNodeTranslator.h"

#include <context/layer.h>
#include <core/boxCollider.h>
#include <physics/physicsWorld.h>

namespace phi
{
    class translationService
    {
    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        collisionNodeTranslator* _nodeTranslator;
        vector<translationPlane*> _translationPlanes;
        vector<translationPlane*> _planesToRemove;
        
        bool _isTranslating;
        bool _showingGhost;
        vector<node*> _ghostNodes;

        physicsWorld* _physicsWorld;
        translationPlane* _currentTranslationPlane;
        translationPlane* _lastChosenTranslationPlane;
        ivec2 _lastMousePosition;

        plane _offsetPlane;

        vector<sweepCollision>* _lastTranslationTouchs;

    private:

        vec3 getClosestAxisNormal(vec3 direction);
        plane createPlane(vec3 direction);
        void createAxisAlignedTranslationPlane(ivec2 position);
        translationPlane* createTranslationPlane(vec3 origin, vec3 normal);
        translationPlane* createTranslationPlane(plane plane, boxCollider* colidee, boxCollider* collider, clippingDistance::clippingDistance clippingDistance);

        void initializeNodeTranslator();
        void endNodeTranslator();
        void createClippingPlanes(translationPlane* clippingTranslationPlane, clippingDistance::clippingDistance clippingDistance);
        void removeClippingPlanes(translationPlane* planeToRemove);
        void addPlanesIfNeeded(vector<sweepCollision> touchs);
        void enqueuePlaneForRemoval(translationPlane* planeToRemove);
        void removeInvalidPlanes();
        void deleteRemovedPlanes();
        void updateTranslationPlanesVisibility();

        vector<sweepCollision> findTouchingCollisions();
        vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);

        bool existsTranslationPlaneWithNormal(vec3 normal);
        bool canTranslateAt(float planeVisibility, float planeExtinctionFactor);
        bool isDraggingObjectIntersectingAnyObject();

        float getPlaneVisibility(plane plane);

        void showGhost();
        void hideGhost();
        void translateGhost(vec3 position, vec3 offset);
        node* cloneNodeAsGhost(node * node);

        vec3 getTranslationOffset(ivec2 mousePosition);
        void translatePlaneGrid(translationPlane* translationPlane, vec3 offset);

    public:
        translationService(
            const vector<node*>* targetNodes, 
            layer* layer, 
            physicsWorld* physicsWorld);

        ~translationService();

        bool isTranslating() const { return _isTranslating; }
        size_t getTargetNodesCount() const { return _targetNodes->size(); }

        void startTranslation(ivec2 mousePosition);
        void translate(ivec2 mousePosition);
        void endTranslation();
        void cancelTranslation();
        void update();
    };
}
