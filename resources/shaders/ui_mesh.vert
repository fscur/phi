#version 330

in vec3 inPosition;

uniform mat4 mvp;

void main(void)
{
     gl_Position = mvp * vec4(inPosition, 1.0);
}