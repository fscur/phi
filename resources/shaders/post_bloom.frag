#version 330

in vec2 fragTexCoord;

uniform sampler2D tex1;
uniform vec2 res;

out vec4 fragColor;

void main(void)
{
	vec4 color = texture(tex1, fragTexCoord);
	float intensity = 1.0 - step(dot(color.rgb, vec3(0.3333, 0.3333, 0.3333)), 0.9);

	fragColor = color * vec4(intensity);
}