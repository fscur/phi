#pragma once
#include <phi.h>
#include "component.h"
#include "transform.h"
#include "obb.h"

namespace phi
{
    class node
    {
    private:
        struct nodeEventTokens
        {
            eventToken transformChangedToken;
            eventToken localObbChangedToken;
        };

    private:
        node* _parent;
        transform* _transform;
        vector<component*> _components;
        vector<node*> _children;
        string _name;
        obb* _localObb;
        obb* _obb;
        bool _isSelected;
        bool _isTranslating;
        unordered_map<node*, nodeEventTokens> _childrenEventTokens;

    public:
        eventHandler<node*> childAdded;
        eventHandler<node*> childRemoved;
        eventHandler<node*> transformChanged;
        eventHandler<node*> selectionChanged;
        eventHandler<node*> localObbChanged;

    private:
        void updateObb();
        void raiseTransformChanged(transform* transform);
        void onChildTransformChanged(node* child);
        void onLocalObbChanged(node * child);

    public:
        CORE_API node(string name = string(""));
        CORE_API node(const node& original);
        CORE_API ~node();

        string getName() const { return _name; }
        transform* getTransform() const { return _transform; }
        node* getParent() const { return _parent; }
        const vector<node*>* getChildren() const { return &_children; }
        const vector<component*>* getComponents() const { return &_components; }
        const obb* const getLocalObb() const { return _localObb; }
        const obb* const getObb() const { return _obb; }
        bool isSelected() const { return _isSelected; }
        bool getIsTranslating() const { return _isTranslating; }

        CORE_API void setIsTranslating(bool value);
        CORE_API void setParent(node* const value);
        CORE_API void setPosition(vec3 value);
        CORE_API void setSize(vec3 value);
        CORE_API void setLocalObb(obb* value);

        CORE_API bool operator ==(const node& other);
        CORE_API bool operator !=(const node& other);

        CORE_API node* clone() const;
        CORE_API void addComponent(component* const component);
        CORE_API void addChild(node* const child);
        CORE_API void removeChild(node* child);
        CORE_API void clearChildren();
        CORE_API void select();
        CORE_API void deselect();
        CORE_API void traverse(std::function<void(node*)> func);

        template<typename T>
        T* getComponent() const
        {
            const component::componentType type = T::getComponentType();
            for (auto& component : _components)
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

            for (auto& child : _children)
                child->traverse(func);
        }

        template<typename T>
        void traverseNodesContaining(std::function<void(node*, T*)> func)
        {
            auto component = getComponent<T>();
            if (component)
                func(this, component);

            for (auto& child : _children)
                child->traverseNodesContaining(func);
        }
    };
}