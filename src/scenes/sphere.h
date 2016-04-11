#pragma once
#include <phi.h>
#include "scenesApi.h"
#include <rendering/mesh.h>

namespace phi
{
    class sphere :
        public mesh
    {
    private:
        float _radius;

    private:
        sphere(float radius, phi::geometry* geometry, phi::material* material);

    private:
        static phi::geometry* createSphereGeometry(uint rings, uint sectors);

    public:
        SCENES_API static sphere* create(float radius, uint rings, uint sectors, phi::material* material);

    public:
        SCENES_API ~sphere();

        float getRadius() const { return _radius; }
        void setRadius(float radius);
    };
}