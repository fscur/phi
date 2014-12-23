#version 330

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;
in vec3 inTangent;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 mvp;
uniform mat4 itmv;
uniform float time;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragTangent;
out vec3 fragSunPos;
out vec4 fragSkyColor;
out vec4 fragHorizonColor;

float PI = 3.14159265358979323846264;
float SEC_IN_RADIANS = 0.00007272205216643039903;
float T = 1.0/24.0;

float getLightChanellValue(float a, float b, float c, float d, float e, float f, float g, float h)
{
    float x = 2.0 * h - 1.0;
    float p = pow(abs(x), b);
    float t = pow(x, g);
    float angle = PI * a + PI * p * c;
    float cossine = cos(angle);
    return (((cossine + 1.0))/((f * t) + 1.0)) * d + e;
}

vec4 getHorizonColor(float t)
{
	float r = GetLightChanellValue(0.0, 0.0, 0.0, 2.0, -0.1, 800.0, 8.0, t * T);
	float g = GetLightChanellValue(0.0, 0.0, 0.0, 2.0, -0.7, 800.0, 8.0, t * T);
	float b = GetLightChanellValue(0.0, 0.0, 0.0, 1.0, 0.2, 300000.0, 14.0, t * T);

	return vec4(clamp(r, 0.0, 1.0), clamp(g, 0.0, 1.0), clamp(b, 0.0, 1.0), 1.0);
}

vec4 getSkyColor(float t)
{
	float r = GetLightChanellValue(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, t * T);
	float g = GetLightChanellValue(1.0, 1.0, 1.0, -1.0, 0.7, 1.0, 0.0, t * T);
	float b = GetLightChanellValue(1.0, 1.0, 0.8, -1.0, 1.0, 1.0, 0.0, t * T);

	return vec4(clamp(r, 0.0, 1.0), clamp(g, 0.0, 1.0), clamp(b, 0.0, 1.0), 1.0);
}

vec3 getSunPos(float t)
{
	float alpha = (t - 21600) * SEC_IN_RADIANS;
	float beta = 0;
	
    float x = cos(beta) * cos(alpha);
    float y = sin(alpha);
    float z = -sin(beta) * cos(alpha);

    return normalize(vec3(x,y,z));
}

void main(void)
{
	vec4 position = viewMatrix * modelMatrix * vec4(inPosition, 1.0);
	gl_Position = mvp * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragNormal = (itmv * vec4(inNormal, 0.0)).xyz;
	fragTangent = (itmv * vec4(inTangent, 0.0)).xyz;
	fragPosition = position.xyz;

	//float t = time;

	fragSunPos = GetSunPos(time * 3600);
	fragHorizonColor = GetHorizonColor(time);
	fragSkyColor = GetSkyColor(time);
}