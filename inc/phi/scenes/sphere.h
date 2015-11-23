#ifndef _PHI_SPHERE_H_
#define _PHI_SPHERE_H_

#include <glm\glm.hpp>
#include "phi/scenes/sceneObject.h"
#include "phi/core/globals.h"

namespace phi
{
    class sphere
    {
    private:
        float _radius;
        model* _model;

    private:
        sphere(){};
        mesh* create(GLuint rings, GLuint sectors);
    public:
        SCENES_API sphere(float radius, int rings, int sectors, material* material);
        SCENES_API ~sphere(void);

        SCENES_API float getRadius() const { return _radius; }
        SCENES_API model* getModel() const { return _model; }

        SCENES_API void setRadius(float radius);
    };
}

#endif