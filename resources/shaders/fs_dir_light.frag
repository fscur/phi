#version 330

struct material
{
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 emissiveColor;
	float kd;
	float ks;
	float shininess;
	float isEmissive;
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
uniform sampler2D emissiveMap;

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
	float diffuseFactor = mat.kd * light.intensity * max(0.0, dot(n, s));
	float specFactor =  mat.ks * pow(max(0.0, dot(n,h)), mat.shininess);

	vec4 diffuseColorMap = texture(diffuseMap, fragTexCoord);
	vec4 specularColorMap = texture(specularMap, fragTexCoord);
	vec4 emissiveColorMap = texture(emissiveMap, fragTexCoord.xy);
	
	//vec4 emissiveComponent = emissiveColorMap * mat.emissiveColor * mat.isEmissive;
	vec4 diffuseComponent = light.color * diffuseColorMap * mat.diffuseColor * diffuseFactor;
	vec4 specularComponent = specularColorMap * mat.specularColor * specFactor;
	
	vec4 color = diffuseComponent + specularComponent;
	fragColor = vec4(color.rgb, mat.isEmissive * emissiveColorMap.r);

	//fragColor = vec4(fragColor.rgb, 1.0);
	//fragColor = vec4(0.0, 0.0, 1.0, 1.0);
}

