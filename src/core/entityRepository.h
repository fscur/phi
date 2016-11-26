#pragma once
#include <phi.h>
#include "entity.h"

namespace phi
{
    template<typename T>
    class entityRepository
    {
    private:
        vector<T*> _entities;

    public:
        entityRepository()
        {
        }

        ~entityRepository()
        {
            release();
        }

        void release()
        {
            for(auto entity : _entities)
                safeDelete(entity);
        }

        void add(T* entity)
        {
            _entities.push_back(entity);
        }

        T* getEntity(int index)
        {
            return _entities[index];
        }

        T* getEntity(guid guid) const
        {
            for (auto entity : _entities)
            {
                if (entity->getGuid() == guid)
                    return entity;
            }

            return nullptr;
        }
    };
}