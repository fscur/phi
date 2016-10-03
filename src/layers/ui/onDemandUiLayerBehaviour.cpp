#include <precompiled.h>
#include <diagnostic/stopwatch.h>
#include <input/input.h>
#include <ui/switchControlBuilder.h>
#include <ui/layoutTransform.h>
#include <rendering/defaultFramebuffer.h>
#include <rendering/fontsManager.h>

#include "onDemandUiLayerBehaviour.h"

namespace phi
{
    onDemandUiLayerBehaviour::onDemandUiLayerBehaviour(layer* uiLayer, layer* targetLayer) :
        _uiLayer(uiLayer),
        _targetLayer(targetLayer),
        _uiCamera(_uiLayer->getCamera()),
        _targetCamera(_targetLayer->getCamera()),
        _font(fontsManager::defaultFont)
    {
        _targetLayer->addOnNodeSelectionChanged(std::bind(&onDemandUiLayerBehaviour::onNodeSelectionChanged, this, std::placeholders::_1));
        _targetCamera->getTransform()->getChangedEvent()->assign(std::bind(&onDemandUiLayerBehaviour::onTargetCameraChanged, this, std::placeholders::_1));
    }

    void onDemandUiLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        if (node->isSelected())
        {
            _uiNodeDatas[node].button = createUi(node);

            //auto worldPositionAtMouse = _targetCamera->screenPointToWorld(mousePosition.x, mousePosition.y);
            auto mousePosition = input::getMousePosition();

            _uiNodeDatas[node].targetWorldPosition = vec3(mousePosition, 0.0f);

            _uiNodeDatas[node].button->getTransform()->setLocalPosition(vec3(getPositionAtNode(node), 0.0f));

            _uiLayer->add(_uiNodeDatas[node].button);
        }
        else
        {
            auto uiNodes = _uiNodeDatas[node];
            _uiNodeDatas.erase(node);

            uiNodes.button->getParent()->removeChild(uiNodes.button);
            safeDelete(uiNodes.button);
        }
    }

    void onDemandUiLayerBehaviour::onTargetCameraChanged(transform* transform)
    {
        for (auto pair : _uiNodeDatas)
        {
            auto uiNodeData = pair.second;

            auto position = getPositionAtNode(pair.first);
            uiNodeData.button->setPosition(vec3(position, 0.0f));
        }
    }

    node* onDemandUiLayerBehaviour::createUi(node* node)
    {
        auto switchControl = switchControlBuilder::newSwitchControl()
            .withSize(vec3(15.0f, 30.0f, 0.1f))
            .build();

        return switchControl;
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

        auto uiNodeSize = uiNodeData.button->getTransform()->getSize();

        return vec2(uiCameraWorldPosition.x, uiCameraWorldPosition.y - uiNodeSize.y * 0.5f);
    }
}