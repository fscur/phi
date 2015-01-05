#ifndef _PHI_SKY_DOME_H_
#define _PHI_SKY_DOME_H_

#include "sceneObject.h"
#include "globals.h"

namespace phi
{
    class skyDome :
        public sceneObject
    {
	private:
		float _radius;
    private:
		skyDome(){};
		mesh* create(GLuint rings, GLuint sectors);
    public:
        SCENES_API skyDome(float radius, int rings, int sectors, material* material);
        SCENES_API ~skyDome(void);

		SCENES_API float getRadius() const { return _radius; }
		SCENES_API void setRadius(float radius);

		SCENES_API void render();
    };
}

#endif