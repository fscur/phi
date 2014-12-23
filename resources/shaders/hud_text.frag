#version 330

in vec2 fragTexCoord;

uniform sampler2D textTexture;

out vec4 fragColor;

void main(void)
{
	fragColor = texture(textTexture, fragTexCoord);
}