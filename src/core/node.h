#pragma once
#include <phi.h>
#include "component.h"
#include "transform.h"
#include <vector>

namespace phi
{
    class node
    {
    private:
        node* _parent;
        transform* _transform;
        vector<component*> _components;
        vector<node*> _children;
        eventHandler<node*>* _transformChanged;

    private:
        void transformChanged(transform* sender);

    public:
        CORE_API node();
        CORE_API node(const node& original);
        CORE_API ~node();

        CORE_API node* clone() const;

        CORE_API void addComponent(component* const component);
        CORE_API void addChild(node* const child);

        CORE_API void traverse(std::function<void(node*)> func);

        transform* getTransform() const { return _transform; }
        node* getParent() const { return _parent; }
        vector<node*>& getChildren() { return _children; }
        eventHandler<node*>* getTransformChanged() { return _transformChanged; }
        void setParent(node* const value) { _parent = value; }

        CORE_API void setPosition(vec3 value);
        CORE_API void setSize(vec3 value);

        template<typename T>
        T* getComponent() const
        {
            const component::componentType type = T::getComponentType();

            for (unsigned int i = 0; i < _components.size(); i++)
            {
                if (_components[i]->getType() == type)
                    return static_cast<T*>(_components[i]);
            }

            return nullptr;
        }
    };
}