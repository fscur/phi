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
        RENDERING_API sphere(glm::vec3 position, float radius, int rings, int sectors, material* material);
        RENDERING_API ~sphere(void);

		RENDERING_API void setRadius(float radius);
		RENDERING_API float getRadius() const { return _radius; }
    };
}

#endif