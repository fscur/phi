#version 330

struct material
{
	vec4 ambientColor;
	float ka;
};

in vec2 fragTexCoord;

uniform vec4 ambientLightColor;
uniform material mat;
uniform sampler2D diffuseMap;

out vec4 fragColor;

void main(void)
{
	//fragColor = ambientLightColor;
	fragColor = mat.ambientColor * mat.ka * ambientLightColor * texture(diffuseMap, fragTexCoord.xy);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}