	#version 330

in vec2 fragTexCoord;

uniform sampler2D tex;

uniform vec2 res;
uniform vec2 blurScale;

out vec4 fragColor;

void main(void)
{
	vec4 color = vec4(0.0);

	color += texture(tex, fragTexCoord + (vec2(-3.0) * blurScale)) * 0.015625;
	color += texture(tex, fragTexCoord + (vec2(-2.0) * blurScale)) * 0.09375;
	color += texture(tex, fragTexCoord + (vec2(-1.0) * blurScale)) * 0.234375;
	color += texture(tex, fragTexCoord + (vec2(0.0) * blurScale)) * 0.3125;
	color += texture(tex, fragTexCoord + (vec2(1.0) * blurScale)) * 0.234375;
	color += texture(tex, fragTexCoord + (vec2(2.0) * blurScale)) * 0.09375;
	color += texture(tex, fragTexCoord + (vec2(3.0) * blurScale)) * 0.015625;

	fragColor = color;

	//fragColor = texture(tex, fragTexCoord);
}