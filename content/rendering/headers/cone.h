#ifndef _PHI_CONE_H_
#define _PHI_CONE_H_

#include "sceneObject.h"
#include "globals.h"

namespace phi
{
    class cone :
        public sceneObject
    {
	private:
		float _radius;
		float _height;
    private:
		cone(){};
		mesh* create(unsigned int sectors);
    public:
        RENDERING_API cone(glm::vec3 position, float height, float radius, unsigned int sectors, material* material);
        RENDERING_API ~cone(void);
		
		RENDERING_API float getRadius() const { return _radius; }
		RENDERING_API float getHeight() const { return _height; }

		RENDERING_API void setRadius(float radius);
		RENDERING_API void setHeight(float height);

		RENDERING_API void debugRender() override;
    };
}

#endif