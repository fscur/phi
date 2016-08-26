#pragma once

#include <phi.h>

#include <core\node.h>
#include <rendering\camera.h>

#include "inputController.h"
#include "layer.h"
#include "translationPlane.h"

namespace phi
{
    class translationInputController :
        public inputController
    {
    private:
        const vector<node*>* _targetNodes;
        layer* _layer;
        camera* _camera;
        vector<translationPlane*> _translationPlanes;
        bool _isTranslating;
        vector<translationPlane*> _planesToRemove;

    private:
        vec3 getClosestAxisNormal(vec3 direction);
        translationPlane* createTranslationPlane(vec3 origin, vec3 normal);
        bool canStartTranslation(mouseEventArgs* e);
        void startTranslation(ivec2 mousePosition);
        void endTranslation();
        void cancelTranslation();
        void enqueuePlaneForRemoval(translationPlane* planeToRemove);

    public:
        translationInputController(const vector<node*>* targetNodes, layer* layer);
        ~translationInputController();

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;
        bool update() override;
        void cancel() override;
    };
}