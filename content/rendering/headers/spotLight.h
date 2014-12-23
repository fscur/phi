#ifndef _PHI_SPOT_LIGHT_H_
#define _PHI_SPOT_LIGHT_H_

#include "pointLight.h"
#include "cone.h"

namespace phi
{
    class spotLight :
        public light
    {
    private:
		attenuation _attenuation;
        float _range;
        glm::vec3 _direction;
        float _cutoff;
		float _radius;
		cone* _boundingVolume;

	private:
		float calcRadius(float cutoff, float range);

    public:
        RENDERING_API spotLight();
        RENDERING_API spotLight(glm::vec3 position, color color, float intensity, attenuation attenuation, glm::vec3 direction, float cutoff);
        RENDERING_API ~spotLight();
    
		RENDERING_API attenuation getAttenuation() const { return _attenuation; }
		RENDERING_API glm::vec3 getDirection() const { return _direction; }
		RENDERING_API float getCutoff() const { return _cutoff; }
		RENDERING_API float getRadius() const { return _radius; }
		RENDERING_API float getRange() const { return _range; }
		RENDERING_API cone* getBoundingVolume() const { return _boundingVolume; }

		RENDERING_API void setAttenuation(attenuation attenuation);
		RENDERING_API void setDirection(glm::vec3 direction);
		RENDERING_API void setCutoff(float cutoff);

		RENDERING_API void update() override;
    };
}
#endif