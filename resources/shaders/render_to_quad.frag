#version 330

in vec2 fragTexCoord;

uniform sampler2D quadTexture;

out vec4 fragColor;

void main(void)
{	
    fragColor = texture(quadTexture, fragTexCoord);
}