#ifndef _PHI_SPOT_LIGHT_H_
#define _PHI_SPOT_LIGHT_H_

#include "phi/scenes/pointLight.h"
#include "phi/scenes/cone.h"

namespace phi
{
    class spotLight :
        public light
    {
    private:
        mat4 _projectionMatrix;
        mat4 _viewMatrix;

        float _range;
        float _oneOverRangeSqr;
        float _cutoff;
        float _radius;
        cone* _boundingVolume;

    private:
        float calcRadius(float cutoff, float range);
        void updateViewMatrix();
        void updateProjectionMatrix();

    public:
        SCENES_API spotLight(vec3 position, color color, float intensity, float range, vec3 direction, float cutoff);
        SCENES_API ~spotLight();
    
        SCENES_API float getCutoff() const { return _cutoff; }
        SCENES_API float getRadius() const { return _radius; }
        SCENES_API float getRange() const { return _range; }
        SCENES_API float getOneOverRangerSqr() const { return _oneOverRangeSqr; }
        SCENES_API cone* getBoundingVolume() const { return _boundingVolume; }

        SCENES_API void setRange(float value);
        SCENES_API void setCutoff(float value);
        SCENES_API void setIntensity(float value) override;

        SCENES_API void update() override;
        
		SCENES_API transform* getTransform() override;
    };
}
#endif