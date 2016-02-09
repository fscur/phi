#version 330

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 mvp;
uniform mat4 itmv;
uniform float time;
uniform float sunOrientation;
uniform vec4 horizonColor;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPosition;

out vec3 fragWorldPos;
out vec3 fragTangent;
out vec3 fragSunPos;
out vec4 fragSkyColor;
out vec4 fragHorizonColor;

float PI = 3.14159265358979323846264;
float SEC_IN_RADIANS = 0.00007272205216643039903;
float T = 0.041666666667;

float getLightChanellValue(float a, float b, float c, float d, float e, float f, float g, float h)
{
    float r = 2.0 * h - 1.0;
    float p = pow(abs(r), b);
    float q = pow(abs(r), g);
    float angle = PI * a + PI * p * c;
    float cossine = cos(angle);
    return ((cossine + 1.0)/((f * q) + 1.0)) * d + e;
}

float getLightChanellValue2(float a, float b, float c, float d, float e)
{
    float t0 = (2.0 * e) - 1.0; //0 -> 1
    float f = pow(abs(t0), d);
    //return t0;
    return (1.0 / ((c * f) + 1.0)) * a + b;
}

vec4 getHorizonColor(float t)
{
	float r = getLightChanellValue2(0.9, 0.1, 8000.0, 16.0, t * T);
	float g = getLightChanellValue2(0.9, 0.1, 9000.0, 12.0, t * T);
	float b = getLightChanellValue2(0.8, 0.2, 1000.0, 8.0, t * T);

	return vec4(clamp(r, 0.0, 1.0), clamp(g, 0.0, 1.0), clamp(b, 0.0, 1.0), 1.0);
}

vec4 getSkyColor(float t)
{
	float r = getLightChanellValue(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, t * T);
	float g = getLightChanellValue(1.0, 1.0, 1.0, -1.0, 0.7, 1.0, 0.0, t * T);
	float b = getLightChanellValue(1.0, 1.0, 0.8, -1.0, 1.0, 1.0, 0.0, t * T);

	return vec4(clamp(r, 0.0, 1.0), clamp(g, 0.0, 1.0), clamp(b, 0.0, 1.0), 1.0);
}

vec3 getSunPos(float t)
{
	float alpha = (t - 21600) * SEC_IN_RADIANS;
	float beta = sunOrientation;
	
    float x = cos(beta) * cos(alpha);
    float y = sin(alpha);
    float z = -sin(beta) * cos(alpha);

    return normalize(vec3(x,y,z));
}

void main(void)
{
	vec4 position = v * m * vec4(inPosition, 1.0);
	vec4 glPos = mvp * vec4(inPosition, 1.0);
	gl_Position = glPos.xyzw;

	fragTexCoord = inTexCoord;
	fragNormal = inNormal;
	fragWorldPos = inPosition;
	fragPosition = position.xyz;

	fragSunPos = getSunPos(time * 3600);
	//fragHorizonColor = getHorizonColor(time);
	fragHorizonColor = horizonColor;
	fragSkyColor = getSkyColor(time);
}