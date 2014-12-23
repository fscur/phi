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
        RENDERING_API skyDome(float radius, int rings, int sectors, material* material);
        RENDERING_API ~skyDome(void);

		RENDERING_API float getRadius() const { return _radius; }
		RENDERING_API void setRadius(float radius);

		RENDERING_API void render();
    };
}

#endif