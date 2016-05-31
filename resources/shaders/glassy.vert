#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in mat4 inModelMatrix;

layout (location = 0) uniform mat4 v;
layout (location = 1) uniform mat4 p;

out vec2 fragTexCoord;
flat out uint instanceId;

void main()
{
    gl_Position = p * v * inModelMatrix * vec4(inPosition, 1.0);

	fragTexCoord = inTexCoord;
    instanceId = gl_InstanceID;
}