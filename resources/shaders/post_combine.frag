#version 330

in vec2 fragTexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform vec2 res;

out vec4 fragColor;

void main(void)
{
	vec4 color = texture(tex1, fragTexCoord) + texture(tex2, fragTexCoord);

	fragColor = color;
}