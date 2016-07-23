#pragma once

#include <phi.h>

#include <core\node.h>

namespace phi
{
    class nodeTranslator
    {
    protected:
        bool _hasBegun;
        node* _node;

    public:
        nodeTranslator() {}
        ~nodeTranslator() {}

        virtual void setNode(node* node)
        {
            _node = node;
        }

        virtual void beginTranslations() { }

        virtual void translateNode(vec3 offset)
        {
            _node->getTransform()->translate(offset);
        }

        virtual void endTranslations() { }
    };
}