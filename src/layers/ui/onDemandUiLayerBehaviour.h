#pragma once

#include <phi.h>
#include <core/node.h>
#include <context/layer.h>
#include <rendering/font.h>
#include <layers/layersApi.h>

namespace phi
{
    class onDemandUiLayerBehaviour
    {
    private:
        struct uiNodes
        {
            node* button01;
            node* button02;
            node* button03;
            node* button04;
            node* button05;
            node* button06;
        };

    public:
        onDemandUiLayerBehaviour(layer* uiLayer, layer* targetLayer);

    private:
        node* createButtonFor(wstring text, node* node, float sphereAngle);
        vec2 getPositionAtNodeSphere(node* node, float sphereAngle);
        void onNodeSelectionChanged(node* node);
        void onTargetCameraChanged(transform* transform);

    private:
        layer* _uiLayer;
        layer* _targetLayer;
        camera* _uiCamera;
        camera* _targetCamera;
        font* _font;
        unordered_map<node*, uiNodes> _uiNodes;
    };
}