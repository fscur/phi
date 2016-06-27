#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

void main()
{
    mat4 mvp = frameUniforms.p * frameUniforms.v * inModelMatrix;
    gl_Position = mvp * vec4(inPosition, 1.0);
}