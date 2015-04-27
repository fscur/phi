#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec3 fragSunPos;
in vec4 fragSkyColor;
in vec4 fragHorizonColor;
in float fragAlpha;

uniform mat4 v;
uniform float time;
uniform float radius;

out vec4 fragColor;

float PI = 3.14159265358979323846264;
float SECS = 0.00001157407;

float getHorizonFactor(vec3 normal)
{
	vec3 up = vec3(0.0, 1.0, 0.0);
	float factor = clamp(dot(-normal, up), 0.0, 1.0);
	return pow(1.0 - factor, 16.0);
	//return pow(clamp(factor, 0.0, 1.0), 0.2);
}

float getSkyGlowFactor(vec3 normal)
{
	vec3 sunPos = (vec4(fragSunPos, 0.0)).xyz;
	vec3 sunNormal = -normalize(sunPos);
	float skyGlowFactor = pow(min(dot(normal, sunNormal) + 1.5, 1.0), 1.0);

	return skyGlowFactor;
}

float getSunGlowFactor(vec3 normal)
{
	float factor = (((time * 3600.0) - 21600.0) * SECS) * 2.0 * PI;								
	float alpha = 1.0 - clamp(abs(cos(factor)), 0.0, 0.3);
	vec3 sunPos = (vec4(fragSunPos, 0.0)).xyz;
	vec3 sunNormal = -normalize(sunPos);
	float sunGlowFactor = pow(max(dot(normal, sunNormal), 0.0), 32 * alpha);

	return sunGlowFactor * 0.5;
}

vec4 getSunColor(vec3 normal)
{
	vec4 color = vec4(0.8, 0.5, 0.2, 1.0);
	float dist = length(normalize(fragWorldPos) - fragSunPos);
	return color * (1.0 - smoothstep(0.015, 0.02, dist));
}

vec4 getMixedColor(vec4 color1, vec4 color2, float factor)
{
    float a = length(color2);
    float b = length(factor * color2);
    
    float c = b/a;
    
    return mix(color1, color2, c);
}

vec4 applyFog(vec4 rgb, float distance ) // camera to point distance
{
    float b = 0.005;
    float fogAmount = 1.0 - exp( -distance*b );
    vec4  fogColor  = vec4(0.5,0.6,0.7, 1.0);
    return mix( rgb, fogColor, fogAmount );
}

void main(void)
{	
	vec3 normal = normalize(fragNormal);

	float horizonFactor = getHorizonFactor(normal);
	float sunGlowFactor = getSunGlowFactor(normal);
	float skyGlowFactor = getSkyGlowFactor(normal);

	vec4 color = fragSkyColor * vec4(0.5, 0.5, 0.5, 1.0);

	color = getMixedColor(color, fragSkyColor, skyGlowFactor);
	color = getMixedColor(color, fragHorizonColor, horizonFactor);
	color = getMixedColor(color, fragHorizonColor, sunGlowFactor);

	vec4 sunColor = getSunColor(normal);

	if (fragWorldPos.y < 0)
	{
		//float factor = clamp(dot(fragSunPos, vec3(0.0, 1.0, 0.0)), 0.0, 1.0);

		//fragColor = vec4(factor);
		fragColor = vec4(0.5,0.6,0.7, 1.0);
	}
	else

		fragColor = color + sunColor;

	//fragColor = applyFog(fragColor, abs(fragPosition.z));

	//fragColor = fragHorizonColor;
}