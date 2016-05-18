#include <precompiled.h>
#include "glyphNode.h"

namespace phi
{
    inline glyphNode::glyphNode(rectangle rect) :
        _left(nullptr),
        _right(nullptr),
        _glyph(nullptr),
        rect(rect) 
    {
    }

    glyphNode::~glyphNode()
    {
        safeDelete(_left);
        safeDelete(_right);
    }

    inline bool glyphNode::isLeaf()
    {
        return _left == nullptr && _right == nullptr;
    }

    glyphNode* glyphNode::insert(glyph* glyph, glyphNode* node)
    {
        glyphNode* newNode = nullptr;

        if (!isLeaf())
        {
            newNode = _left->insert(glyph);

            if (newNode != nullptr)
                return newNode;

            return _right->insert(glyph);
        }
        else
        {
            if (node->_glyph != nullptr)
                return nullptr;

            auto w = static_cast<int>(glyph->bitmapWidth);
            auto h = static_cast<int>(glyph->bitmapHeight);
            auto glyphRect = rectangle(0, 0, w, h);

            if (!node->rect.contains(glyphRect))
                return nullptr;

            if (glyphRect.w == rect.w &&
                glyphRect.h == rect.h)
            {
                node->_glyph = glyph;
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

                node->_left = new glyphNode(leftRect);
                node->_right = new glyphNode(rightRect);
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

                node->_left = new glyphNode(leftRect);
                node->_right = new glyphNode(rightRect);
            }

            return insert(glyph, _left);
        }
    }

    inline glyphNode* glyphNode::insert(glyph* glyph)
    {
        return insert(glyph, this);
    }
}