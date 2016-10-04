#include <precompiled.h>
#include <input/input.h>
#include <ui/layoutTransform.h>
#include <diagnostic/stopwatch.h>

#include "onDemandUiLayerBehaviour.h"

namespace phi
{
    onDemandUiLayerBehaviour::onDemandUiLayerBehaviour(layer* uiLayer, layer* targetLayer, std::function<node*()> createUiFunction) :
        _uiLayer(uiLayer),
        _targetLayer(targetLayer),
        _createUiFunction(createUiFunction),
        _uiCamera(_uiLayer->getCamera()),
        _targetCamera(_targetLayer->getCamera())
    {
        _targetLayer->addOnNodeSelectionChanged(std::bind(&onDemandUiLayerBehaviour::onNodeSelectionChanged, this, std::placeholders::_1));
        _targetCamera->getTransform()->getChangedEvent()->assign(std::bind(&onDemandUiLayerBehaviour::onTargetCameraChanged, this, std::placeholders::_1));
    }

    void onDemandUiLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        if (node->isSelected())
        {
            _uiNodeDatas[node].uiRoot = _createUiFunction();
            _uiNodeDatas[node].uiRoot->getTransform()->setLocalPosition(vec3(getPositionAtNode(node), 0.0f));
            _uiLayer->add(_uiNodeDatas[node].uiRoot);
            _uiNodeDatas[node].isInLayer = true;
        }
        else
        {
            auto uiNodes = _uiNodeDatas[node];
            _uiNodeDatas.erase(node);

            uiNodes.uiRoot->getParent()->removeChild(uiNodes.uiRoot);
            safeDelete(uiNodes.uiRoot);
        }
    }

    void onDemandUiLayerBehaviour::onTargetCameraChanged(transform* transform)
    {
        for (auto pair : _uiNodeDatas)
        {
            auto uiNodeData = pair.second;
            auto position = getPositionAtNode(pair.first);
            uiNodeData.uiRoot->setPosition(vec3(position, 0.0f));
        }
    }

    vec2 onDemandUiLayerBehaviour::getPositionAtNode(node* node)
    {
        auto obb = node->getObb();
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

        auto uiNodeData = _uiNodeDatas[node];
        auto screenPosition = vec2(maxCornerOnScreen.x + 50, minCornerOnScreen.y + (maxCornerOnScreen.y - minCornerOnScreen.y) * 1.0f);

        auto uiCameraViewPosition = _uiCamera->screenPointToView(static_cast<int>(screenPosition.x), static_cast<int>(_uiCamera->getResolution().height - screenPosition.y), _uiCamera->getTransform()->getPosition().z);
        auto uiCameraWorldPosition = mathUtils::multiply(glm::inverse(_uiCamera->getViewMatrix()), uiCameraViewPosition);

        auto uiNodeSize = uiNodeData.uiRoot->getTransform()->getSize();

        return vec2(uiCameraWorldPosition.x, uiCameraWorldPosition.y - uiNodeSize.y * 0.0f);
    }
}