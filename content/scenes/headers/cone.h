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
        SCENES_API cone(glm::vec3 position, float height, float radius, unsigned int sectors, material* material);
        SCENES_API ~cone(void);
		
		SCENES_API float getRadius() const { return _radius; }
		SCENES_API float getHeight() const { return _height; }

		SCENES_API void setRadius(float radius);
		SCENES_API void setHeight(float height);

		SCENES_API void debugRender() override;
    };
}

#endif