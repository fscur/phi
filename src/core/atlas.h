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
        atlasNode* insert(atlasItem* item, atlasNode* node);

    public:
        atlasNode(rectangle<uint> rect);
        ~atlasNode();

        atlasNode* insert(atlasItem* item);

        CORE_API bool isLeaf();
    };

    class atlas
    {
    private:
        atlasNode* _root;
        bool _isFull;

    public:
        CORE_API atlas(sizeui size);
        CORE_API ~atlas();

        CORE_API atlasNode* insert(atlasItem* item);
        
        CORE_API bool isFull() const { return _isFull; }
        CORE_API bool isEmpty() const { return _root->isLeaf(); }
    };
}