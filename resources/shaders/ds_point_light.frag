#version 330

struct attenuation
{
	float constant;
	float linear;
	float exponential;
};

struct pointLight
{
	vec3 position;
	vec4 color;
	float intensity;
	attenuation attenuation;
	float range;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragLight;
in vec3 fragEye;
in vec3 fragTangent;

uniform mat4 m;
uniform mat4 v;
uniform vec2 res;

uniform pointLight light;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D shininessMap;

out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0);

	vec2 texCoord = gl_FragCoord.xy / res;
	vec3 fragPosition = texture(positionMap, texCoord).xyz;
   	vec3 normal = texture(normalMap, texCoord).xyz;
   	vec4 diffuseColor = texture(diffuseMap, texCoord);
   	vec4 specularColor = texture(specularMap, texCoord);
   	float shininess = texture(shininessMap, texCoord).x;
	vec3 lightDir = fragPosition - (v * vec4(light.position, 1.0)).xyz;
	float distanceToPoint = length(lightDir);

	lightDir = normalize(lightDir);
	
	vec3 s = normalize(-lightDir);
	vec3 v = normalize(-fragPosition);
	vec3 h = normalize(v+s);
	float diffuse = light.intensity * max(0.0, dot(normal, s));
	float spec = pow(max(0.0, dot(normal,h)), shininess);

	fragColor = light.color * diffuseColor * diffuse + light.color *specularColor * spec;

	float attenuation = light.attenuation.constant + 
			light.attenuation.linear * distanceToPoint +
			light.attenuation.exponential * distanceToPoint * distanceToPoint + 
			0.0001;

	fragColor = fragColor / attenuation;// + vec4(0.5, 0.0, 0.0, 1.0);

	//fragColor = vec4(1.0);
	//fragColor = vec4(fragPosition, 1.0);
	//fragColor = vec4(gl_FragCoord.xy, 1.0, 1.0);
}