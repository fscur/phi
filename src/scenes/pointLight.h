#pragma once

#include <rendering/attenuation.h>

#include "light.h"
#include "sphere.h"

namespace phi
{
    class pointLight :
        public light
    {
    private:
        sphere* _boundingVolume;
        float _range;
        float _oneOverRangeSqr;
        unsigned int _boundingVolumeSides;

    private:
        float calcRange(float radius, unsigned int sides);

    public:

        SCENES_API pointLight(vec3 position, color color, float intensity, float range);
        SCENES_API ~pointLight();

        SCENES_API sphere* getBoundingVolume() const { return _boundingVolume; }
        SCENES_API float getRange() const { return _range; }
        SCENES_API float getOneOverRangerSqr() const { return _oneOverRangeSqr; }

        SCENES_API void setIntensity(float value) override;
        SCENES_API void setRange(float value);
    };
}