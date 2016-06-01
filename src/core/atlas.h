#pragma once
#include <phi.h>
#include "coreApi.h"
#include "rectangle.h"

namespace phi
{
    struct atlasItem
    {
        rectangle rect;
        const void* object;

        atlasItem(rectangle rect, const void* object) :
            rect(rect),
            object(object)
        {}
    };

    class atlasNode
    {
    private:
        atlasNode* _left;
        atlasNode* _right;
        atlasItem* _item;

    public:
        rectangle rect;

    private:
        bool isLeaf();
        atlasNode* insert(atlasItem* item, atlasNode* node);

    public:
        atlasNode(rectangle rect);
        ~atlasNode();

        atlasNode* insert(atlasItem* item);
    };

    class atlas
    {
    private:
        atlasNode* _root;

    public:
        CORE_API atlas(int width, int height);

        CORE_API atlasNode* insert(atlasItem* item);
    };
}