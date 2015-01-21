#version 330

in vec2 fragTexCoord;

uniform int id;
uniform float isSelected;
uniform sampler2D diffuseMap;
uniform vec4 diffuseColor;

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

void main()	
{	
	fragColor = diffuseColor * texture(diffuseMap, fragTexCoord.xy);
    selectionMap = vec4 (getIdColor(id), isSelected);
    //selectionMap = vec4 (isSelected, 0.0, 0.0, 0.5);
}