#version 330

in vec2 fragTexCoord;

uniform vec4 selectionColor;
uniform sampler2D diffuseMap;
uniform vec4 diffuseColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 selectionMap;

void main()	
{	
	fragColor = diffuseColor * texture(diffuseMap, fragTexCoord.xy);
    selectionMap = selectionColor;
    //selectionMap = vec4 (isSelected, 0.0, 0.0, 0.5);
}