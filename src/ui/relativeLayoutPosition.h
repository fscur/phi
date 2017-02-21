#pragma once

#include <phi.h>
#include <core/component.h>
#include <core/node.h>
#include <rendering/camera.h>
#include <layers/layer.h>
#include "uiApi.h"

namespace phi
{
    class relativeLayoutPosition :
        public component
    {
    public:
        static componentType getComponentType() { return componentType::RELATIVE_LAYOUT_POSITION; }

    public:
        UI_API relativeLayoutPosition(camera* camera, layer* targetLayer);
        UI_API relativeLayoutPosition(const relativeLayoutPosition& original);
        UI_API ~relativeLayoutPosition();

        UI_API component* clone() const override;

        UI_API node* getTargetNode() const;
        UI_API void setTargetNode(node* value);

        UI_API void updatePosition();

    private:
        vec3 getRelativePositionToTargetNode();
        void onTargetCameraChanged(transform* transform);

    private:
        camera* _camera;
        layer* _targetLayer;
        camera* _targetCamera;
        node* _targetNode;
    };
}