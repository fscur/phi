#version 330

in vec3 inPosition;
in vec2 inTexCoord;

out vec2 fragTexCoord;

void main(void)
{
	gl_Position = vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
}