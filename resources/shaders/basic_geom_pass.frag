#version 450

in vec2 fragTexCoord;

uniform vec4 selectionColor;
uniform vec4 diffuseColor;
uniform sampler2DArray textures;
uniform int diffuseTextureIndex;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 selectionMap;

void main()
{
	fragColor = diffuseColor * texture(textures, vec3(fragTexCoord.xy, float(diffuseTextureIndex)));
    selectionMap = selectionColor;
    //selectionMap = vec4 (isSelected, 0.0, 0.0, 0.5);
}