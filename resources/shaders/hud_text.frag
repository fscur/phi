#version 330

in vec2 fragTexCoord;

uniform sampler2D texture;
uniform vec2 res;
uniform vec4 color;
uniform vec2 texCoordOrigin;
uniform vec2 texCoordQuadSize;
uniform vec2 texSize;

out vec4 fragColor;

void main(void)
{
	vec2 uv = texCoordOrigin + fragTexCoord * texCoordQuadSize;
	fragColor = vec4(1.0, 1.0, 1.0, texture2D(texture, uv).r) * color;
}