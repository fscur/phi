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
            node* uiRoot;
            bool isInLayer;
        };

    public:
        onDemandUiLayerBehaviour(layer* uiLayer, layer* targetLayer, std::function<node*()> createUiFunction);

    private:
        vec2 getPositionAtNode(node* node);
        void onNodeSelectionChanged(node* node);
        void onTargetCameraChanged(transform* transform);

    private:
        layer* _uiLayer;
        layer* _targetLayer;
        std::function<node*()> _createUiFunction;
        camera* _uiCamera;
        camera* _targetCamera;
        unordered_map<node*, uiNodeData> _uiNodeDatas;
    };
}