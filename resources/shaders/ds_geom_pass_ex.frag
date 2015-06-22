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

uniform vec4 selectionColor;
uniform vec4 ambientLightColor;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

layout (location = 0) out vec4 rt0; 
layout (location = 1) out vec4 rt1;
layout (location = 2) out vec4 rt2; 
layout (location = 3) out vec4 rt3; 

//http://aras-p.info/texts/CompactNormalStorage.html
//method 7 - stereographic projection

vec2 encodeNormal (vec3 n)
{
    float scale = 1.7777;
    vec2 enc = n.xy / (n.z + 1);
    enc /= scale;
    enc = enc * 0.5 + 0.5;
    return vec2(enc);
}

void main()	
{	
    //normal map
	vec3 n = normalize(fragNormal);
	vec3 t = normalize(fragTangent); 
	vec3 b = cross(n, t);

	mat3 tbn = mat3(t, b, n);
	n = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

	vec2 normal = encodeNormal(n);

    vec4 diffuseColor = texture(diffuseMap, fragTexCoord) * mat.diffuseColor;
    vec4 ambientColor = diffuseColor * ambientLightColor * mat.ambientColor * mat.ka;
	vec4 specularColor = mat.specularColor * texture(specularMap, fragTexCoord) * mat.ks;

    vec3 c = diffuseColor.rgb * mat.kd + specularColor.rgb;

	rt0 = vec4(ambientColor.rgb, mat.shininess/512.0); //RGBA
    rt1 = vec4(diffuseColor.rgb * mat.kd, normal.x); //RGBA16F
    rt2 = vec4(specularColor.rgb, normal.y); //RGBA16F
    rt3 = selectionColor; //RGBA
}
