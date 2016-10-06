#include <precompiled.h>
#include "relativeLayoutPosition.h"

namespace phi
{
    relativeLayoutPosition::relativeLayoutPosition(camera* camera, layer* targetLayer) :
        component(componentType::RELATIVE_LAYOUT_POSITION),
        _camera(camera),
        _targetLayer(targetLayer),
        _targetCamera(_targetLayer->getCamera()),
        _targetNode(nullptr)
    {
        _targetCamera->getTransform()->getChangedEvent()->assign(std::bind(&relativeLayoutPosition::onTargetCameraChanged, this, std::placeholders::_1));
    }

    relativeLayoutPosition::relativeLayoutPosition(const relativeLayoutPosition& original) :
        component(componentType::RELATIVE_LAYOUT_POSITION),
        _camera(original._camera),
        _targetLayer(original._targetLayer),
        _targetCamera(original._targetCamera),
        _targetNode(original._targetNode)
    {
    }

    relativeLayoutPosition::~relativeLayoutPosition()
    {
    }

    component* relativeLayoutPosition::clone() const
    {
        return new relativeLayoutPosition(*this);
    }

    node* relativeLayoutPosition::getTargetNode() const
    {
        return _targetNode;
    }

    void relativeLayoutPosition::setTargetNode(node* value)
    {
        _targetNode = value;

        if (_targetNode)
        {
            auto position = getRelativePositionToTargetNode();
            getNode()->setPosition(vec3(position, 0.0f));
        }
    }

    void relativeLayoutPosition::onTargetCameraChanged(transform* transform)
    {
        if (_targetNode)
        {
            auto position = getRelativePositionToTargetNode();
            getNode()->setPosition(vec3(position, 0.0f));
        }
    }

    vec2 relativeLayoutPosition::getRelativePositionToTargetNode()
    {
        auto obb = _targetNode->getObb();
        auto obbCorners = obb->getCorners();
        auto maxFloat = std::numeric_limits<float>().max();
        auto minFloat = std::numeric_limits<float>().min();
        auto minCornerOnScreen = vec2(maxFloat, maxFloat);
        auto maxCornerOnScreen = vec2(minFloat, minFloat);
        for (size_t i = 0; i < 8; i++)
        {
            auto cornerOnScreen = _targetCamera->worldPointToScreen(obbCorners[i]);
            maxCornerOnScreen.x = glm::max(maxCornerOnScreen.x, cornerOnScreen.x);
            maxCornerOnScreen.y = glm::max(maxCornerOnScreen.y, cornerOnScreen.y);
            minCornerOnScreen.x = glm::min(minCornerOnScreen.x, cornerOnScreen.x);
            minCornerOnScreen.y = glm::min(minCornerOnScreen.y, cornerOnScreen.y);
        }

        auto screenPosition = vec2(maxCornerOnScreen.x + 50, minCornerOnScreen.y + (maxCornerOnScreen.y - minCornerOnScreen.y) * 1.0f);

        auto uiCameraViewPosition = _camera->screenPointToView(static_cast<int>(screenPosition.x), static_cast<int>(_camera->getResolution().height - screenPosition.y), _camera->getTransform()->getPosition().z);
        auto uiCameraWorldPosition = mathUtils::multiply(glm::inverse(_camera->getViewMatrix()), uiCameraViewPosition);

        auto uiNodeSize = getNode()->getTransform()->getSize();

        return vec2(uiCameraWorldPosition.x, uiCameraWorldPosition.y - uiNodeSize.y * 0.0f);
    }
}