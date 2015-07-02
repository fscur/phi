#version 330

in vec2 fragTexCoord;

uniform sampler2D diffuseMap;
uniform vec4 diffuseColor;

out vec4 fragColor;

void main()
{
	fragColor = diffuseColor * texture(diffuseMap, fragTexCoord.xy);
	//o fernando zuou tudo
}