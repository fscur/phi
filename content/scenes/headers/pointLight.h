#ifndef _PHI_POINT_LIGHT_H_
#define _PHI_POINT_LIGHT_H_

#include "light.h"
#include "attenuation.h"
#include "sphere.h"

namespace phi
{
    class pointLight :
        public light
    {
    private:
		sphere* _boundingVolume;
        attenuation _attenuation;
        float _range;

	private:
		float calcRange();

	protected:
		pointLight(){};
    public:
		
        SCENES_API pointLight(glm::vec3 position, color color, float intensity, attenuation attenuation);
        SCENES_API ~pointLight();
    
        SCENES_API attenuation getAttenuation();
        SCENES_API void setAttenuation(attenuation attenuation);
		SCENES_API void setPosition(glm::vec3 position) override;

		SCENES_API sphere* getBoundingVolume() const { return _boundingVolume; }
		SCENES_API float getRange() const { return _range; }

		SCENES_API void update() override;
    };
}
#endif