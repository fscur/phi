#version 330

in vec2 fragTexCoord;

uniform sampler2D tex1;
uniform vec2 res;

out vec4 fragColor;

void main(void)
{
	vec4 color = texture2D(tex1, fragTexCoord);
	float emissive = color.a;
	fragColor = vec4(color.rgb * color.a, color.a);
}