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
        physicsWorld* _physicsWorld;
        collisionNodeTranslator* _nodeTranslator;

        vector<translationPlane*> _translationPlanes;
        vector<translationPlane*> _planesToRemove;
        
        bool _isTranslating;

        translationPlane* _currentTranslationPlane;
        ivec2 _lastMousePosition;
        plane _offsetPlane;

        /*translationPlane* _lastChosenTranslationPlane;
        bool _showingGhost;
        vector<node*> _ghostNodes;
        vector<sweepCollision>* _lastTranslationTouchs;*/

    private:
        vec3 getClosestAxisTo(vec3 direction);
        plane createPlaneFromAxis(vec3 axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);

        translationPlane* createTranslationPlane(vec3 origin, vec3 normal);

        void addTranslationPlane(translationPlane * translationPlane);
        void enqueuePlaneForRemoval(translationPlane* planeToRemove);
        void removeInvalidPlanes();
        void deleteRemovedPlanes();
        void updateTranslationPlanesVisibility();

        bool canTranslateAt(float planeVisibility, float planeExtinctionFactor);
        float getPlaneVisibility(plane plane);

        void translateTargetNodes(vec3 endPosition);
        void translatePlaneGrid(vec3 endPosition);

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

        //vector<sweepCollision> findTouchingCollisions();
        //vector<sweepCollision> getValidTouchCollisions(vector<sweepCollision>& touchs);
        //translationPlane* createTranslationPlane(plane plane, boxCollider* colidee, boxCollider* collider, clippingDistance::clippingDistance clippingDistance);
        /*void createClippingPlanes(translationPlane* clippingTranslationPlane, clippingDistance::clippingDistance clippingDistance);
        void removeClippingPlanes(translationPlane* planeToRemove);
        void addPlanesIfNeeded(vector<sweepCollision> touchs);*/
        /*bool existsTranslationPlaneWithNormal(vec3 normal);
        bool isDraggingObjectIntersectingAnyObject();*/
        /*void showGhost();
        void hideGhost();
        void translateGhost(vec3 position, vec3 offset);
        node* cloneNodeAsGhost(node * node);*/
    };
}
