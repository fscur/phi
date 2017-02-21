#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec4 inSelectionColor;
layout (location = 3) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsDataBuffer
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
    vec2 resolution;
    float near;
    float far;
    float halfFovTangent;
    float time; 
    float pad0;
    float pad1;
} frameUniforms;

out vec2 fragTexCoord;
flat out uint instanceId;
flat out vec4 selectionColor;

void main()
{
    gl_Position = frameUniforms.p * frameUniforms.v * inModelMatrix * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord;
    instanceId = gl_InstanceID;
    selectionColor = inSelectionColor;
}