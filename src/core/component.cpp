#include "precompiled.h"
#include "component.h"

namespace phi
{
    void component::setNode(node * value)
    {
        auto previousValue = _node;
        _node = value;
        onNodeChanged(previousValue);
    }

    bool component::operator==(const component& other)
    {
        return _name == other._name &&
               _type == other._type;
    }

    bool component::operator!=(const component& other)
    {
        return !(*this == other);
    }
}
