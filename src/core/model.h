#pragma once
#include <phi.h>

#include "guid.h"
#include "entity.h"
#include "node.h"

namespace phi
{
    class model :
        public entity
    {
    private:
        node* _node;

    public:
        model(guid guid, node* node) :
            entity::entity(guid),
            _node(node)
        {
        }

        model(const model& original) :
            entity::entity(guid::newGuid()),
            _node(original._node->clone())
        {
        }

        node* getNode() { return _node; }

        transform* getTransform() { return _node->getTransform(); }

        model* clone() const
        {
            auto m = static_cast<const model*>(this);
            return new model(*m);
        }
    };
}