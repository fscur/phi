#pragma once
#include "phi.h"

namespace phi
{
    template<typename Key, typename Value>
    class linkedList
    {
    public:
        struct node
        {
            node(Key key, Value value) :
                key(key),
                value(value),
                next(nullptr)
            {
            }

            Key key;
            Value value;
            node* next;
        };

    private:
        node* _root;
        node* _last;
        size_t _entriesCount;

    private:
        void removeNode(node * nodeToRemove, node * previousNode)
        {
            if (previousNode)
                previousNode->next = nodeToRemove->next;

            if (nodeToRemove == _last)
                _last = previousNode;

            free(nodeToRemove);
            _entriesCount--;
        }

    public:
        linkedList() :
            _root(nullptr),
            _last(nullptr),
            _entriesCount(0)
        {
        }

        ~linkedList()
        {
            node* currentNode = _root;
            node* lastNode = nullptr;
            while (currentNode)
            {
                lastNode = currentNode;
                currentNode = currentNode->next;
                free(lastNode);
            }
        }

        void add(Key key, Value value)
        {
            auto node = phi::make<linkedList::node>(key, value);

            if (!_root)
            {
                _root = node;
                _last = _root;
                _entriesCount++;
                return;
            }

            _last->next = node;
            _last = _last->next;
            _entriesCount++;
        }

        bool remove(Key key)
        {
            if (!_root)
                return false;

            if (_root->key == key)
            {
                if (!_root->next)
                {
                    free(_root);
                    _root = nullptr;
                    _last = nullptr;
                    _entriesCount--;
                }
                else
                {
                    auto entry = _root;
                    _root = _root->next;

                    if (_root == _last)
                        _last = _root->next;

                    free(entry);
                    _entriesCount--;
                }

                return true;
            }

            node* currentNode = _root;
            node* previousNode = nullptr;

            while (currentNode)
            {
                if (currentNode->key == key)
                {
                    removeNode(currentNode, previousNode);
                    return true;
                }

                previousNode = currentNode;
                currentNode = currentNode->next;
            }

            return false;
        }

        node* getEntries() { return _root; }
        size_t getEntriesCount() { return _entriesCount; }
    };
}