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

out vec3 fragTangent;
out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragLight;
out vec3 fragEye;
out vec2 fragTexCoord;

void main(void)
{
	vec4 position = v * m * vec4(inPosition, 1.0);
	gl_Position = mvp * vec4(inPosition, 1.0);
	fragNormal = (itmv * vec4(inNormal, 0.0)).xyz;
	fragTangent = (itmv * vec4(inTangent, 0.0)).xyz;
	fragTexCoord = inTexCoord;
	fragPosition = position.xyz;
}