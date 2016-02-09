#version 330

in vec2 fragTexCoord;

uniform sampler2D quadTexture;
uniform vec4 backColor;

out vec4 fragColor;

void main(void)
{
	fragColor = texture(quadTexture, fragTexCoord) * backColor;
}