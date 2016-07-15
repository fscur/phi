#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

layout (location = 0) uniform mat4 m;
layout (location = 1) uniform float size;

out vec2 fragTexCoord;

void main()
{
    mat4 mvp = frameUniforms.p * frameUniforms.v * m;
    gl_Position = mvp * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord * size;
}