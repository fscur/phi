#version 330

in vec3 inPosition;

uniform mat4 mvp;
uniform mat4 m;

uniform vec3 lightPos;

out vec3 fragWorldPos;
out vec3 fragLightDir;

void main(void)
{
	gl_Position = mvp * vec4(inPosition, 1.0);

    fragWorldPos = (m * vec4(inPosition, 1.0)).xyz; 
    fragLightDir = fragWorldPos - lightPos;
}