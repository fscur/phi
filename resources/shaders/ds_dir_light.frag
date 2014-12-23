#version 330

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
in vec4 fragLightSpacePos;

uniform mat4 v;

uniform directionalLight light;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D shininessMap;

out vec4 fragColor;

void main(void)
{
	vec3 fragPosition = texture(positionMap, fragTexCoord).rgb;
   	vec3 normal = texture(normalMap, fragTexCoord).rgb;
   	vec4 diffuseColor = texture(diffuseMap, fragTexCoord);
   	vec4 specularColor = texture(specularMap, fragTexCoord);
   	float shininess = texture(shininessMap, fragTexCoord).r;
   	//change mat3(viewMatrix) by viewMatrix * vec4s
	vec3 lightDir = mat3(v) * normalize(light.direction);

	vec3 s = normalize(-lightDir);
	vec3 fp = normalize(-fragPosition);
	vec3 h = normalize(fp+s);
	float diffuse = light.intensity * max(0.0, dot(normal, s));
	float spec = pow(max(0.0, dot(normal,h)), shininess);

	fragColor = light.color * diffuseColor * diffuse + light.color * specularColor * spec;
	//fragColor = vec4(fragTexCoord, 0.0, 1.0);
	//fragColor = vec4(fragPosition, 1.0);
	//fragColor = fragPos;
	//fragColor =  vec4(1.0);
}