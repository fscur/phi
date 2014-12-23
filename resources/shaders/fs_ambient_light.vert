#version 330

in vec3 inPosition;
in vec2 inTexCoord;

uniform mat4 mvp;

out vec2 fragTexCoord;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
}