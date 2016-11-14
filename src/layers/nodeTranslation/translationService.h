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
    struct collisionObbPlane
    {
        const obb* colliderObb;
        plane plane;
    };

    class translationService
    {
    public:
        translationService(
            const vector<node*>* targetNodes,
            layer* layer,
            physicsWorld* physicsWorld);

        ~translationService();

        void startTranslation(ivec2 mousePosition, node* clickedNode);
        void translate(ivec2 mousePosition);
        void endTranslation();

        void deleteClippedPlanes();

        void update();

        void disableCollisions();
        void enableCollisions();
        void disablePlaneChanges();
        void enablePlaneChanges();
        void disableSnapToGrid();
        void enableSnapToGrid();

        bool isTranslating() const { return _isTranslating; }

    private:
        void changeToAxisAlignedPlane();
        plane createPlaneFromAxis(const vec3& axis);
        translationPlane* createAxisAlignedTranslationPlane(ivec2 position);
        translationPlane* createTranslationPlane(const plane& plane);

        void enqueuePlaneForDeletion(translationPlane* planeToRemove);
        void deletePlane(translationPlane* translationPlane);
        void deletePlaneIfNotVisible();

        void updatePlaneVisibility(translationPlane* translationPlane);

        bool canChangeTo(const plane& plane);
        bool isPlaneVisible(const plane& plane);
        float getExtinctionFactor(const vec3& normal);
        float getPlaneVisibility(const plane& plane);
        bool isNormalValidForCollision(const sweep::sweepCollision & touch);
        void addClippingPlanes();
        void updateClippingPlanes();

        void tryChangeToPlanesFromCollisions();
        vec3 tryChangeToAttachedPlane(vec3 offset);
        void changePlanes(translationPlane * translationPlane, const plane& offsetPlane);
        void createOffsetOrientedPlane();
        void showTranslationPlane();

        vec3 getSnapOffset(vec3 offset);
        vec3 snapToGrid(vec3 endPosition);
        void getObbLimitsOnOrientedOffsetPlane(vec3 offset, vec2& minimum, vec2& maximum);
        void translateTargetNodes(const vec3 targetPosition);
        void translatePlaneGrid(const vec3& targetPosition);
        void updateDestinationObbs();
        void updateSnapGridSize();

        bool changeToTouchingPlaneIfAble();
        vector<sweep::sweepCollision> findTouchingCollisions();
        vector<sweep::sweepObbCollision> findTouchingCollisionsOnDirection(const vec3& direction, float distance);
        vector<sweep::sweepCollision> filterValidTouchCollisions(vector<sweep::sweepCollision>& touchs);
        vector<sweep::sweepObbCollision> filterValidTouchCollisions(vector<sweep::sweepObbCollision>& touchs);

    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        physicsWorld* _physicsWorld;
        collisionNodeTranslator* _nodeTranslator;
        ghostNodeTranslator* _ghostTranslator;

        bool _isTranslating;
        bool _canChangePlanes;
        bool _isSnapToGridEnabled;

        node* _clickedNode;
        plane _offsetPlane;
        orientedPlane _orientedOffsetPlane;
        translationPlane* _currentTranslationPlane;
        vector<translationPlane*> _planesToDelete;
        ivec2 _lastMousePosition;
        vec3 _collidedDelta;
        vec3 _snappedDelta;
        vector<obb*> _targetNodesDestinationObbs;
        vector<sweep::sweepObbCollision> _lastTranslationTouchingCollisions;
        float _snapGridSize;

        unordered_map<const obb*, vector<translationPlane*>> _clippedTranslationPlanes;
    };
}
