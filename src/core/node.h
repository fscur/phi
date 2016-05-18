#pragma once
#include <phi.h>
#include "component.h"
#include "transform.h"

namespace phi
{
    class node
    {
    private:
        node* _parent;
        transform* _transform;
        vector<component*>* _components;
        vector<node*>* _children;
        string _name;

    public:
        eventHandler<node*> childAdded;
        eventHandler<node*> childRemoved;
        eventHandler<node*> transformChanged;

    private:
        void raiseTransformChanged(transform* sender);

    public:
        CORE_API node(string name = string(""));
        CORE_API node(const node& original);
        CORE_API ~node();
        CORE_API node* clone() const;

        CORE_API void addComponent(component* const component);
        CORE_API void addChild(node* const child);
        CORE_API void removeChild(node* child);
        CORE_API void clearChildren();

        string getName() const { return _name; }
        transform* getTransform() const { return _transform; }
        node* getParent() const { return _parent; }
        vector<node*>* getChildren() const { return _children; }
        vector<component*>* getComponents() const { return _components; }

        CORE_API void setParent(node* const value);
        CORE_API void setPosition(vec3 value);
        CORE_API void setSize(vec3 value);
        CORE_API void traverse(std::function<void(node*)> func);

        template<typename T>
        T* getComponent() const
        {
            const component::componentType type = T::getComponentType();
            for (auto component : *_components)
            {
                if (component->getType() == type)
                    return static_cast<T*>(component);
            }

            return nullptr;
        }

        template<typename T>
        void traverse(std::function<void(T*)> func)
        {
            auto component = getComponent<T>();
            if (component)
                func(component);

            for (auto child : *_children)
                child->traverse(func);
        }

        template<typename T>
        void traverseNodesContaining(std::function<void(node*, T*)> func)
        {
            auto component = getComponent<T>();
            if (component)
                func(this, component);

            for (auto child : *_children)
                child->traverseNodesContaining(func);
        }
    };
}