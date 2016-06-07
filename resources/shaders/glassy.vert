#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

out vec2 fragTexCoord;
flat out uint instanceId;

void main()
{
    gl_Position = frameUniforms.p * frameUniforms.v * inModelMatrix * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord;
    instanceId = gl_InstanceID;
}