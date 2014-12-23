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

struct directionalLight
{
	vec3 position;
	vec4 color;
	float intensity;
	vec3 direction;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragLight;
in vec3 fragEye;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform mat4 v;
uniform mat4 m;

uniform material mat;
uniform directionalLight light;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

out vec4 fragColor;

void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);
	normal = mat3(tangent, bitangent, normal) * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

	vec3 lightDir = mat3(v) * normalize(light.direction);

	vec3 s = normalize(-lightDir);
	vec3 fp = normalize(-fragPosition);
	vec3 n = normal;
	vec3 h = normalize(fp+s);
	float diffuse = mat.kd * light.intensity * max(0.0, dot(n, s));
	float spec =  mat.ks * pow(max(0.0, dot(n,h)), mat.shininess);

	fragColor = light.color * texture(diffuseMap, fragTexCoord) * mat.diffuseColor * diffuse + mat.specularColor * spec;
	
	//fragColor = vec4(fragNormal, 1.0);
	//fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

