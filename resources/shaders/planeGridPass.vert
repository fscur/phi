#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

uniform float scale;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

uniform mat4 m;

out vec2 fragTexCoord;

const float CELL_SIZE = 1.0;

void main()
{
    mat4 mvp = frameUniforms.p * frameUniforms.v * m;
    gl_Position = mvp * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord * scale * CELL_SIZE;
}