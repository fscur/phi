#version 330 

// Testar para remover o layout daqui
layout (location = 0) in vec3 inPosition; 
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent; 

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 itmv;

out vec3 fragNormal; 
out vec3 fragTangent; 
out vec2 fragTexCoord; 
out vec3 fragWorldPos; 

void main()
{ 
    gl_Position = mvp * vec4(inPosition, 1.0);
    fragNormal = (itmv * vec4(inNormal, 0.0)).xyz;
	fragTangent = (itmv * vec4(inTangent, 0.0)).xyz;
    fragTexCoord = inTexCoord;
    fragWorldPos = (mv * vec4(inPosition, 1.0)).xyz;
}