#include <precompiled.h>

#include <ui/buttonBuilder.h>

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
        //_targetLayer->addOnNodeSelectionChanged(std::bind(&onDemandUiLayerBehaviour::onNodeSelectionChanged, this, std::placeholders::_1));
        //_targetCamera->getTransform()->getChangedEvent()->assign(std::bind(&onDemandUiLayerBehaviour::onTargetCameraChanged, this, std::placeholders::_1));
    }

    void onDemandUiLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        if (node->isSelected())
        {
            uiNodes uiNodes;
            uiNodes.button01 = createButtonFor(L"1", node, PI * 0.15f);
            uiNodes.button02 = createButtonFor(L"2", node, 0.0f);
            uiNodes.button03 = createButtonFor(L"3", node, -PI * 0.15f);
            uiNodes.button04 = createButtonFor(L"4", node, PI - PI * 0.15f);
            uiNodes.button05 = createButtonFor(L"5", node, PI);
            uiNodes.button06 = createButtonFor(L"6", node, PI + PI * 0.15f);
            _uiNodes[node] = uiNodes;

            _uiLayer->add(uiNodes.button01);
            _uiLayer->add(uiNodes.button02);
            _uiLayer->add(uiNodes.button03);
            _uiLayer->add(uiNodes.button04);
            _uiLayer->add(uiNodes.button05);
            _uiLayer->add(uiNodes.button06);
        }
        else
        {
            auto uiNodes = _uiNodes[node];
            _uiNodes.erase(node);

            uiNodes.button01->getParent()->removeChild(uiNodes.button01);
            safeDelete(uiNodes.button01);
            uiNodes.button02->getParent()->removeChild(uiNodes.button02);
            safeDelete(uiNodes.button02);
            uiNodes.button03->getParent()->removeChild(uiNodes.button03);
            safeDelete(uiNodes.button03);
            uiNodes.button04->getParent()->removeChild(uiNodes.button04);
            safeDelete(uiNodes.button04);
            uiNodes.button05->getParent()->removeChild(uiNodes.button05);
            safeDelete(uiNodes.button05);
            uiNodes.button06->getParent()->removeChild(uiNodes.button06);
            safeDelete(uiNodes.button06);
        }
    }

    void onDemandUiLayerBehaviour::onTargetCameraChanged(transform* transform)
    {
        for (auto pair : _uiNodes)
        {
            auto uiNodes = pair.second;

            auto position1 = getPositionAtNodeSphere(pair.first, PI * 0.15f);
            auto size1 = uiNodes.button01->getTransform()->getSize();
            uiNodes.button01->setPosition(vec3(position1 + vec2(0.0f, 0.0f), 0.0f));

            auto position2 = getPositionAtNodeSphere(pair.first, 0.0f);
            auto size2 = uiNodes.button02->getTransform()->getSize();
            uiNodes.button02->setPosition(vec3(position2 + vec2(0.0f, -size2.y * 0.5f), 0.0f));

            auto position3 = getPositionAtNodeSphere(pair.first, -PI * 0.15f);
            auto size3 = uiNodes.button03->getTransform()->getSize();
            uiNodes.button03->setPosition(vec3(position3 + vec2(0.0f, -size3.y), 0.0f));

            auto position4 = getPositionAtNodeSphere(pair.first, PI - PI * 0.15f);
            auto size4 = uiNodes.button04->getTransform()->getSize();
            uiNodes.button04->setPosition(vec3(position4 + vec2(-size4.x, 0.0f), 0.0f));

            auto position5 = getPositionAtNodeSphere(pair.first, PI);
            auto size5 = uiNodes.button05->getTransform()->getSize();
            uiNodes.button05->setPosition(vec3(position5 + vec2(-size5.x, -size5.y * 0.5f), 0.0f));

            auto position6 = getPositionAtNodeSphere(pair.first, PI + PI * 0.15f);
            auto size6 = uiNodes.button06->getTransform()->getSize();
            uiNodes.button06->setPosition(vec3(position6 + vec2(-size6.x, -size6.y), 0.0f));
        }
    }

    node* onDemandUiLayerBehaviour::createButtonFor(wstring text, node* node, float sphereAngle)
    {
        auto button = buttonBuilder::newButton()
            .withPosition(vec3(getPositionAtNodeSphere(node, sphereAngle), 0.0f))
            .withText(text)
            .withTextColor(0.7f, 0.7f, 0.7f, 1.0f)
            .withFont(_font)
            .withControlColor(1.0f, 1.0f, 1.0f, 1.0f)
            .withAction([=](phi::node* node)
        {
            phi::debug("clicked!");
        }).build();

        return button;
    }

    vec2 onDemandUiLayerBehaviour::getPositionAtNodeSphere(node* node, float sphereAngle)
    {
        auto targetTransform = _targetCamera->getTransform();
        auto screenXAxis = -targetTransform->getRight();
        auto sphereAngleRotation = glm::angleAxis(sphereAngle, targetTransform->getDirection());

        auto direction = screenXAxis * sphereAngleRotation;

        auto obb = node->getObb();
        auto sphereRadius = glm::length(obb->halfSizes);

        auto screenPosition = _targetCamera->worldPointToScreen(obb->center + direction * sphereRadius);

        auto uiCameraViewPosition = _uiCamera->screenPointToView(static_cast<int>(screenPosition.x), static_cast<int>(_uiCamera->getResolution().height - screenPosition.y), _uiCamera->getTransform()->getPosition().z);
        auto uiCameraWorldPosition = mathUtils::multiply(glm::inverse(_uiCamera->getViewMatrix()), uiCameraViewPosition);

        return vec2(uiCameraWorldPosition.x, uiCameraWorldPosition.y);
    }
}