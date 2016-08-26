#pragma once
#include <phi.h>
#include <context\layer.h>
#include "translationPlane.h"

namespace phi
{
    class translationService
    {
    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        
        vector<translationPlane*> _translationPlanes;
        vector<translationPlane*> _planesToRemove;

        bool _isTranslating;

    private:
        translationPlane * createTranslationPlane(vec3 origin, vec3 normal);
        vec3 getClosestAxisNormal(vec3 direction);
        void enqueuePlaneForRemoval(translationPlane * planeToRemove);

    public:
        translationService(const vector<node*>* targetNodes, layer* layer);
        ~translationService();

        bool isTranslating() const { return _isTranslating; }
        size_t getTargetNodesCount() const { return _targetNodes->size(); }

        void endTranslation();
        void startTranslation(ivec2 mousePosition);
        void cancelTranslation();
        void update();
    };
}
