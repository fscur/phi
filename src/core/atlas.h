#pragma once
#include <phi.h>
#include "coreApi.h"
#include "rectangle.h"
#include "size.h"

namespace phi
{
    struct atlasItem
    {
        rectangle<uint> rect;
        const void* object;

        atlasItem(rectangle<uint> rect, const void* object) :
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
        rectangle<uint> rect;

    private:
        bool isLeaf();
        atlasNode* insert(atlasItem* item, atlasNode* node);

    public:
        atlasNode(rectangle<uint> rect);
        ~atlasNode();

        atlasNode* insert(atlasItem* item);
    };

    class atlas
    {
    private:
        atlasNode* _root;

    public:
        CORE_API atlas(sizeui size);

        CORE_API atlasNode* insert(atlasItem* item);
    };
}