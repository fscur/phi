#include <precompiled.h>
#include "nodeOptimizer.h"

namespace phi
{
    void nodeOptimizer::removeEmptyNodes(node* node)
    {
        auto children = node->getChildren();

        vector<phi::node*> toRemove;

        for (auto child : *children)
        {
            if (isNodeEmpty(child))
                toRemove.push_back(child);

            removeEmptyNodes(child);
        }

        for (auto nodeToRemove : toRemove)
            node->removeChild(nodeToRemove);

        deleteNodes(toRemove);
    }

    node* nodeOptimizer::removeUselessNodes(node* node)
    {
        vector<phi::node*> toRemove;

        auto current = node;
        auto currentTransform = current->getTransform();
        auto nodeParent = node->getParent();

        while (isNodeUseless(current))
        {
            if (nodeParent)
                nodeParent->removeChild(current);

            toRemove.push_back(current);
            current = current->getChildren()->at(0);
            current->getTransform()->multiply(*currentTransform);
            currentTransform = current->getTransform();

            if (nodeParent)
                nodeParent->addChild(current);
        }

        if (current != node)
        {
            deleteNodes(toRemove);
            current->setParent(nodeParent);
        }

        auto children = current->getChildren();
        auto childrenCount = children->size();

        for (size_t i = 0u; i < childrenCount; ++i)
        {
            auto child = children->at(i);
            auto optimizedChild = nodeOptimizer::optimize(child);
            if (child != optimizedChild)
            {
                childrenCount--;
                i--;
            }
        }

        return current;
    }

    void nodeOptimizer::deleteNodes(vector<node*>& nodes)
    {
        for (auto node : nodes)
            node->clearChildren();

        for (auto node : nodes)
            safeDelete(node);
    }

    bool nodeOptimizer::isNodeEmpty(phi::node* node)
    {
        return node->getComponents()->size() == 0 && node->getChildren()->size() == 0;
    }

    bool nodeOptimizer::isNodeUseless(phi::node* node)
    {
        return node->getComponents()->size() == 0 && node->getChildren()->size() == 1;
    }

    node* nodeOptimizer::optimize(node* node)
    {
        removeEmptyNodes(node);
        return removeUselessNodes(node);
    }
}