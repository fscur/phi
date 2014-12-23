#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragSunPos;
in vec4 fragSkyColor;
in vec4 fragHorizonColor;

uniform mat4 v;
uniform float time;
uniform float radius;

uniform sampler2D normalMap;

out vec4 fragColor;

float getHorizonFactor(vec3 normal)
{
	vec3 up = normalize((viewMatrix * vec4(0.0, 1.0, 0.0, 0.0)).xyz);

	return 1.0 - pow(max(dot(normal, -up), 0.0), 0.4);
}

float getSkyGlowFactor(vec3 normal)
{
	vec3 sunPos = (viewMatrix * vec4(fragSunPos, 0.0)).xyz;
	vec3 sunNormal = -normalize(sunPos);
	float skyGlowFactor = pow(min(dot(normal, sunNormal) + 1.5, 1.0), 1.0);

	return skyGlowFactor;
}

float getSunGlowFactor(vec3 normal)
{
	vec3 sunPos = (viewMatrix * vec4(fragSunPos, 0.0)).xyz;
	vec3 sunNormal = -normalize(sunPos);
	float sunGlowFactor = pow(max(dot(normal, sunNormal), 0.0), 16);

	return sunGlowFactor * 0.5;
}

vec4 getSunColor(vec3 normal, vec3 sunPos)
{
	vec4 color = vec4(0.8, 0.5, 0.2, 1.0);
	vec3 sunDir = fragPosition - sunPos;
	float radius = 3.0;
	float dist = length(sunDir);
	return color * (1.0 - smoothstep(radius * 0.8, radius, dist));
}

vec4 getMixedColor(vec4 color1, vec4 color2, float factor)
{
    float a = length(color2);
    float b = length(factor * color2);
    
    float c = b/a;
    
    return mix(color1, color2, c);
}

void main(void)
{
	vec3 normal = normalize(fragNormal);
	vec3 sunPos = (viewMatrix * vec4(fragSunPos * radius, 1.0)).xyz;

	float horizonFactor = GetHorizonFactor(normal);
	float sunGlowFactor = GetSunGlowFactor(normal);
	float skyGlowFactor = GetSkyGlowFactor(normal);

	vec4 color = fragSkyColor * vec4(0.5, 0.5, 0.5, 1.0);

	color = GetMixedColor(color, fragSkyColor, skyGlowFactor);
	color = GetMixedColor(color, fragHorizonColor, horizonFactor);
	color = GetMixedColor(color, fragHorizonColor, sunGlowFactor);
	vec4 sunColor = GetSunColor(normal, sunPos);
	

	fragColor = color + sunColor;
	
	//fragColor = skyColor;
	//fragColor = vec4(normal, 1.0);
	//fragColor = vec4(normalize(normal), 1.0);
	//fragColor = vec4(y, 0, 0, 1);
}