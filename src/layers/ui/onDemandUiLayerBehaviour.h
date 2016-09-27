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
        struct uiNodeData
        {
            vec3 targetWorldPosition;
            node* button;
        };

    public:
        onDemandUiLayerBehaviour(layer* uiLayer, layer* targetLayer);

    private:
        node* createUi(node* node);
        vec2 getPositionAtNode(node* node);
        void onNodeSelectionChanged(node* node);
        void onTargetCameraChanged(transform* transform);

    private:
        layer* _uiLayer;
        layer* _targetLayer;
        camera* _uiCamera;
        camera* _targetCamera;
        font* _font;
        unordered_map<node*, uiNodeData> _uiNodeDatas;
    };
}