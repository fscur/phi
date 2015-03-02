#version 330

struct material
{
	vec4 ambientColor;
	float ka;
};

in vec2 fragTexCoord;
uniform int id;
uniform float isSelected;

uniform vec4 ambientLightColor;
uniform material mat;
uniform sampler2D diffuseMap;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 selectionMap;

vec3 getIdColor(int id)
{
	int r = id & 255;
    id = id >> 8;
    int g = id & 255;
    id = id >> 8;
    int b = id & 255;

    return vec3(r/255.0, g/255.0, b/255.0);
}

void main(void)
{
	//fragColor = ambientLightColor;
	fragColor = mat.ambientColor * mat.ka * ambientLightColor * texture(diffuseMap, fragTexCoord.xy);
	
	//fragColor = vec4(fragColor.rgb, 1.0);
	
    selectionMap = vec4 (getIdColor(id), isSelected);
}