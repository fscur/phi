#version 330

struct material
{
	vec4 ambientColor;
	vec4 emissiveColor;
	float ka;
	float isEmissive;
};

in vec2 fragTexCoord;

uniform vec4 ambientLightColor;
uniform vec4 selectionColor;
uniform material mat;
uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 selectionMap;

void main(void)
{
	vec4 emissiveColor = texture(emissiveMap, fragTexCoord.xy);
	vec4 diffuseColor = texture(diffuseMap, fragTexCoord.xy);

	vec4 emissiveComponent = emissiveColor * diffuseColor * mat.isEmissive * mat.emissiveColor;
	vec4 diffuseComponent = mat.ambientColor * mat.ka * ambientLightColor * diffuseColor;
	
	vec4 color = emissiveComponent + diffuseComponent;

	//fragColor = vec4(color.rgb, 1.0);
	fragColor = vec4(color.rgb, emissiveColor.r * mat.isEmissive);
	//fragColor = vec4(mat.isEmissive);

    selectionMap = selectionColor;
}