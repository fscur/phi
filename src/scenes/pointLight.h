#pragma once
#include <phi.h>
#include "scenesApi.h"

#include <rendering/attenuation.h>

#include "light.h"
#include "sphere.h"

namespace phi
{
    class transform;

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
        SCENES_API pointLight(string name, color color, float intensity, float range, transform* transform);
        SCENES_API ~pointLight();

        SCENES_API void setIntensity(float value) override;
        SCENES_API void setRange(float value);

        sphere* getBoundingVolume() const { return _boundingVolume; }
        float getRange() const { return _range; }
        float getOneOverRangerSqr() const { return _oneOverRangeSqr; }
    };
}