#pragma once
#include <phi.h>
#include "scenesApi.h"
#include "pointLight.h"
#include "cone.h"

namespace phi
{
    class transform;

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
        transform* getTransform() const
        {
            if (_node == nullptr)
                return nullptr;

            return _node->getTransform();
        }

    public:
        SCENES_API spotLight(string name, color color, float intensity, float range, float cutoff);
        SCENES_API ~spotLight();

        SCENES_API void setRange(float value);
        SCENES_API void setCutoff(float value);
        SCENES_API void setIntensity(float value) override;
    
        float getCutoff() const { return _cutoff; }
        float getRadius() const { return _radius; }
        float getRange() const { return _range; }
        float getOneOverRangerSqr() const { return _oneOverRangeSqr; }
        cone* getBoundingVolume() const { return _boundingVolume; }
    };
}