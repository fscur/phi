#ifndef _PHI_SPHERE_H_
#define _PHI_SPHERE_H_

#include <glm\glm.hpp>
#include "sceneObject.h"
#include "globals.h"

namespace phi
{
    class sphere :
        public sceneObject
    {
	private:
		float _radius;
    private:
		sphere(){};
		mesh* create(GLuint rings, GLuint sectors);
    public:
        SCENES_API sphere(float radius, int rings, int sectors, material* material);
        SCENES_API ~sphere(void);

		SCENES_API void setRadius(float radius);
		SCENES_API float getRadius() const { return _radius; }
    };
}

#endif