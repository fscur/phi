#include <precompiled.h>
#include "atlas.h"

namespace phi
{
    inline atlasNode::atlasNode(rectangle rect) :
        _left(nullptr),
        _right(nullptr),
        _item(nullptr),
        rect(rect)
    {
    }

    inline atlasNode::~atlasNode()
    {
        safeDelete(_left);
        safeDelete(_right);
    }

    inline bool atlasNode::isLeaf()
    {
        return _left == nullptr && _right == nullptr;
    }

    atlasNode* atlasNode::insert(atlasItem* item, atlasNode* node)
    {
        atlasNode* newNode = nullptr;

        if (!isLeaf())
        {
            newNode = _left->insert(item);

            if (newNode != nullptr)
                return newNode;

            return _right->insert(item);
        }
        else
        {
            if (node->_item != nullptr)
                return nullptr;

            auto w = item->rect.w;
            auto h = item->rect.h;

            auto atlasRect = rectangle(0, 0, w, h);

            if (!node->rect.contains(atlasRect))
                return nullptr;

            if (atlasRect.w == rect.w &&
                atlasRect.h == rect.h)
            {
                node->_item = item;
                return node;
            }

            auto dw = node->rect.w - w;
            auto dh = node->rect.h - h;

            if (dw > dh)
            {
                auto leftRect = rectangle(
                    node->rect.x,
                    node->rect.y,
                    w,
                    node->rect.h);

                auto rightRect = rectangle(
                    node->rect.x + w + 1,
                    node->rect.y,
                    node->rect.w - w - 1,
                    node->rect.h);

                node->_left = new atlasNode(leftRect);
                node->_right = new atlasNode(rightRect);
            }
            else
            {
                auto leftRect = rectangle(
                    node->rect.x,
                    node->rect.y,
                    node->rect.w,
                    h);

                auto rightRect = rectangle(
                    node->rect.x,
                    node->rect.y + h + 1,
                    node->rect.w,
                    node->rect.h - h - 1);

                node->_left = new atlasNode(leftRect);
                node->_right = new atlasNode(rightRect);
            }

            return insert(item, _left);
        }
    }

    inline atlasNode* atlasNode::insert(atlasItem* item)
    {
        return insert(item, this);
    }

    inline atlas::atlas(int width, int height)
    {
        _root = new atlasNode(rectangle(0, 0, width, height));
    }

    inline atlasNode* atlas::insert(atlasItem* item)
    {
        auto node = _root->insert(item);
        return node;
    }
}