#version 330
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragLight;
in vec3 fragEye;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform sampler2D normalMap;

out vec4 fragColor;

vec3 rim(vec3 color, float start, float end, float coef)
{
	vec3 normal = normalize(fragNormal);
	vec3 eye = normalize(-fragPosition.xyz);
	float rim = smoothstep(start, end, 1.0 - dot(normal, eye));
	return clamp(rim, 0.0, 1.0) * coef * color;
}

void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);
	normal = mat3(tangent, bitangent, normal) * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

	fragColor = vec4(rim(vec3(0.0, 1.0, 0.0), 0.8, 1.0, 4.0), 1.0);
	//fragColor = vec4(fragNormal, 1.0);
	//fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

