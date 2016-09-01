
#include <precompiled.h>
#include "ghostNodeTranslator.h"
#include <core/ghostMesh.h>
#include <core/mesh.h>

namespace phi
{
    ghostNodeTranslator::ghostNodeTranslator(layer* layer) :
        _layer(layer),
        _showingGhosts(false),
        _ghostNodes(unordered_map<node*, node*>())
    {
    }

    ghostNodeTranslator::~ghostNodeTranslator()
    {
    }

    void ghostNodeTranslator::enable()
    {
        if (_showingGhosts)
            return;

        _showingGhosts = true;

        for (auto& pair : _ghostNodes)
            _layer->add(pair.second);

        for (auto& pair : _ghostNodes)
        {
            pair.first->traverse([](phi::node* node) {
                node->isTranslating(true);
            });
        }
    }

    void ghostNodeTranslator::disable()
    {
        if (!_showingGhosts)
            return;

        _showingGhosts = false;

        for (auto& pair : _ghostNodes)
        {
            auto ghostNode = pair.second;
            ghostNode->getParent()->removeChild(ghostNode);
        }

        for (auto& pair : _ghostNodes)
        {
            pair.first->traverse([](phi::node* node) {
                node->isTranslating(false);
            });
        }
    }

    void ghostNodeTranslator::translate(vec3 offset)
    {
        if (!_showingGhosts)
            return;

        for (auto& pair : _ghostNodes)
        {
            auto ghostNode = pair.second;

            ghostNode->traverse<phi::ghostMesh>([=](phi::ghostMesh* ghostMesh)
            {
                ghostMesh->setOffset(offset);
            });

            auto nodePosition = pair.first->getTransform()->getLocalPosition();
            ghostNode->getTransform()->setLocalPosition(nodePosition + offset);
        }
    }

    node* ghostNodeTranslator::cloneNodeAsGhost(node* node)
    {
        auto nodeTransform = node->getTransform();
        auto position = nodeTransform->getLocalPosition();
        auto size = nodeTransform->getLocalSize();
        auto orientation = nodeTransform->getLocalOrientation();

        auto clonedNode = new phi::node(node->getName());
        auto clonedNodeTransform = clonedNode->getTransform();

        clonedNodeTransform->setLocalPosition(position);
        clonedNodeTransform->setLocalSize(size);
        clonedNodeTransform->setLocalOrientation(orientation);

        auto mesh = node->getComponent<phi::mesh>();

        if (mesh)
        {
            auto geometry = mesh->getGeometry();
            auto material = mesh->getMaterial();
            auto ghostMesh = new phi::ghostMesh(geometry, material);
            clonedNode->addComponent(ghostMesh);
        }

        for (auto& child : *node->getChildren())
        {
            auto clonedChild = cloneNodeAsGhost(child);
            clonedNode->addChild(clonedChild);
        }

        return clonedNode;
    }

    void ghostNodeTranslator::addRange(const vector<node*>& nodes)
    {
        for (auto& node : nodes)
            _ghostNodes[node] = cloneNodeAsGhost(node);
    }

    void ghostNodeTranslator::clear()
    {
        if (_showingGhosts)
            disable();

        _ghostNodes.clear();
    }
}