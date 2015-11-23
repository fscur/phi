#ifndef _PHI_SKY_DOME_H_
#define _PHI_SKY_DOME_H_

#include "phi/scenes/sceneObject.h"
#include "phi/core/globals.h"
#include "phi/scenes/directionalLight.h"

namespace phi
{
    class skyDome :
        public sceneObject
    {
	private:
		float _radius;
        float _dayTime;
        float _sunOrientation;
        phi::color _horizonColor;
        directionalLight* _sun;
    private:
		skyDome(){};
		mesh* create(GLuint rings, GLuint sectors);
        void skyDome::updateSun();
        float getLightChanellValue2(float a, float b, float c, float d, float e);
        phi::color calcHorizonColor(float t);
    public:
        SCENES_API skyDome(float radius, int rings, int sectors, material* material);
        SCENES_API ~skyDome(void);

		SCENES_API float getRadius() const { return _radius; }
		SCENES_API float getDayTime() const { return _dayTime; }
		SCENES_API float getSunOrientation() const { return _sunOrientation; }
        SCENES_API phi::color getHorizonColor() const { return _horizonColor; }
        SCENES_API directionalLight* getSun() const { return _sun; }

		SCENES_API void setRadius(float value);
        SCENES_API void setDayTime(float value);
        SCENES_API void setSunOrientation(float value);

		SCENES_API void render();
    };
}

#endif