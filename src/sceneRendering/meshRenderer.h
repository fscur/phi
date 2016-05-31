#pragma once
#include <phi.h>

#include <core\mesh.h>

namespace phi
{
    class meshRenderer
    {
    public:
        meshRenderer();
        ~meshRenderer();

        void add(mesh* mesh);
        void update(mesh* mesh);
        void remove(mesh* mesh);
    };
}