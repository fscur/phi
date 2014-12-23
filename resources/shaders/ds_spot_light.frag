#version 330

struct attenuation
{
	float constant;
	float linear;
	float exponential;
};

struct spotLight
{
	vec3 position;
	vec4 color;
	float intensity;
	attenuation attenuation;
	float range;
	vec3 direction;
	float cutoff;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragLight;
in vec3 fragEye;
in vec3 fragTangent;

uniform mat4 m;
uniform mat4 v;
uniform vec2 res;

uniform spotLight light;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D shininessMap;

out vec4 fragColor;

void main(void)
{
	//fragColor = vec4(0.1);
	vec2 texCoord = gl_FragCoord.xy / res;

	vec3 fragPosition = texture(positionMap, texCoord).xyz;
   	vec3 normal = texture(normalMap, texCoord).xyz;
   	vec4 diffuseColor = texture(diffuseMap, texCoord);
   	vec4 specularColor = texture(specularMap, texCoord);
   	float shininess = texture(shininessMap, texCoord).x;

   	normal = normalize(normal);
	
	vec3 lightDir = fragPosition - (v * vec4(light.position, 1.0)).xyz;
	vec3 lightDirNormalized = normalize(lightDir);

    vec3 spotDir = normalize(mat3(v) * light.direction);
	float spotFactor = dot(spotDir, lightDirNormalized);
	float cutoff = light.cutoff;

	if (spotFactor > cutoff)
	{
		float distanceToPoint = length(lightDir);
		
		vec3 s = normalize(-lightDirNormalized);
		vec3 fp = normalize(-fragPosition);
		vec3 h = normalize(fp+s);
		float diffuse = light.intensity * max(0.0, dot(normal, s));
		float spec = pow(max(0.0, dot(normal,h)), shininess);

		fragColor = light.color * diffuseColor * diffuse + light.color * specularColor * spec;

		float attenuation = light.attenuation.constant + 
				light.attenuation.linear * distanceToPoint + 
				light.attenuation.exponential * distanceToPoint * distanceToPoint + 
				0.0001;

		fragColor = fragColor / attenuation;

		float fadeEdgeFactor = 1.0 - ((1.0 - spotFactor)/(1.0 - light.cutoff + 0.0001));
		fragColor *= fadeEdgeFactor;
		//fragColor += vec4(0.0, 0.0, 0.2, 1.0);
	}

	//fragColor += vec4(0.1, 0.1, 0.1, 1.0);
	//fragColor = vec4(0.2);
	//fragColor = diffuseColor;
}