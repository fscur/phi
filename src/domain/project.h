#pragma once
#include <phi.h>

#include <context/scene.h>

#include "domainApi.h"

namespace demon
{
    class project
    {
    public:
        DOMAIN_API project(phi::scene* scene);
        DOMAIN_API ~project();

        DOMAIN_API phi::scene* getScene();

    private:
        phi::scene* _scene;
    };
}