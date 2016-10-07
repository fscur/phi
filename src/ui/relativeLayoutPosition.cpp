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
        updatePosition();
    }

    void relativeLayoutPosition::updatePosition()
    {
        if (!_targetNode)
            return;

        auto position = getRelativePositionToTargetNode();
        getNode()->setPosition(position);
    }

    void relativeLayoutPosition::onTargetCameraChanged(transform* transform)
    {
        updatePosition();
    }

    vec3 relativeLayoutPosition::getRelativePositionToTargetNode()
    {
        auto obb = _targetNode->getObb();
        auto obbCorners = obb->getCorners();
        auto maxFloat = std::numeric_limits<float>().max();
        auto minFloat = std::numeric_limits<float>().lowest();
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

        auto screenPosition = vec2(maxCornerOnScreen.x + 50.0f, maxCornerOnScreen.y);

        screenPosition.x = glm::max(50.0f, glm::min(screenPosition.x, _camera->getResolution().width - 50.0f));
        screenPosition.y = glm::max(50.0f, glm::min(screenPosition.y, _camera->getResolution().height - 50.0f));

        auto depth = -500.0f;
        auto uiCameraViewPosition = _camera->screenPointToView(static_cast<int>(_camera->getResolution().width - screenPosition.x), static_cast<int>(screenPosition.y), depth);
        auto uiCameraWorldPosition = mathUtils::multiply(glm::inverse(_camera->getViewMatrix()), uiCameraViewPosition);

        auto uiNodeSize = getNode()->getTransform()->getSize();
        return uiCameraWorldPosition + vec3(-uiNodeSize.x * 0.5f, uiNodeSize.y * 0.5f, 0.0f);
    }
}