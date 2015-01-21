#version 330

struct material
{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
	float ka;
	float kd;
	float ks;
	float shininess;
};

in vec3 fragNormal; 
in vec3 fragTangent; 
in vec2 fragTexCoord; 
in vec3 fragWorldPos;  

uniform vec4 ambientLightColor;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform float isSelected;

layout (location = 0) out vec3 worldPosOut;
layout (location = 1) out vec3 normalOut;
layout (location = 2) out vec4 ambientOut; 
layout (location = 3) out vec4 diffuseOut; 
layout (location = 4) out vec4 specularOut;
layout (location = 5) out vec4 shininessOut;

void main()	
{	
	/*
    worldPosOut = fragWorldPos;

    //normal map
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

    normalOut = normal;//normalize(normal);	

    //vec4 diffuseColor = ambientLightColor * material.AmbientColor * material.Ka * texture(diffuseMap, fragTexCoord) * material.Kd;
	
	//vec4 diffuseColor = //// * material.Kd;
	
    vec4 diffuseColor = texture(diffuseMap, fragTexCoord) * mat.diffuseColor;
    vec4 ambientColor = diffuseColor * ambientLightColor * mat.ambientColor * mat.ka;
	//vec4 specularColor = vec4(material.Ks);

	vec4 specularColor = mat.specularColor * texture(specularMap, fragTexCoord) * mat.ks;

    ambientOut = ambientColor;
    diffuseOut = diffuseColor * mat.kd;
    specularOut = specularColor;
	shininessOut = vec4(mat.shininess, isSelected, 0.0, 1.0);
*/
	worldPosOut = vec3(1.0, 1.0, 1.0);
	normalOut = vec3(1.0, 1.0, 0.0);
	ambientOut = vec4(1.0, 0.0, 0.0, 1.0);
    diffuseOut = vec4(0.0, 0.0, 1.0, 1.0);
    specularOut = vec4(0.0, 1.0, 1.0, 1.0);
	shininessOut = vec4(1.0, 0.0, 1.0, 1.0);
}
