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

uniform int id;
uniform float isSelected;
uniform vec4 ambientLightColor;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

layout (location = 0) out vec4 fragColor; 
layout (location = 1) out vec4 worldPosOut;
layout (location = 2) out vec4 normalOut; 
layout (location = 3) out vec4 diffuseOut; 
layout (location = 4) out vec4 specularOut;
layout (location = 5) out vec4 shininessOut;
layout (location = 6) out vec4 selectionMap;

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
    worldPosOut = vec4(fragWorldPos, 1.0);

    //normal map
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

    normalOut = vec4(normal, 1.0);//normalize(normal);	

    //vec4 diffuseColor = ambientLightColor * material.AmbientColor * material.Ka * texture(diffuseMap, fragTexCoord) * material.Kd;
	
	//vec4 diffuseColor = //// * material.Kd;
	
    vec4 diffuseColor = texture(diffuseMap, fragTexCoord) * mat.diffuseColor;
    vec4 ambientColor = diffuseColor * ambientLightColor * mat.ambientColor * mat.ka;
	//vec4 specularColor = vec4(material.Ks);

	vec4 specularColor = mat.specularColor * texture(specularMap, fragTexCoord) * mat.ks;

	fragColor = ambientColor;
    diffuseOut = diffuseColor * mat.kd;
    specularOut = specularColor;
	shininessOut = vec4(mat.shininess, 0.0, 0.0, 0.0);
    selectionMap = vec4 (getIdColor(id), isSelected);
}
