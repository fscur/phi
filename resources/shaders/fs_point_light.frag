#version 330

struct material
{
	vec4 diffuseColor;
	vec4 specularColor;
	float kd;
	float ks;
	float shininess;
};

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
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform mat4 v;
uniform mat4 m;

uniform pointLight light;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

out vec4 fragColor;

void main(void)
{
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;
	
	vec3 lightDir = fragPosition - (v * vec4(light.position, 1.0)).xyz;

	float distanceToPoint = length(lightDir);
	
	if (light.range < distanceToPoint)
	{

		fragColor = vec4(0.0);
		return;
	}

	lightDir = normalize(lightDir);

	float diffuseFactor = dot(normal, -lightDir);

	vec4 diffuseColor =  vec4(0.0);
	vec4 specularColor = vec4(0.0);

	if (diffuseFactor > 0)
	{
		vec4 materialDiffuseColor = mat.diffuseColor * texture(diffuseMap, fragTexCoord);
		vec4 diffuseColor = materialDiffuseColor * mat.kd * light.intensity * light.color * diffuseFactor;

		vec3 R = reflect(lightDir, normal);
		vec3 E = normalize(-fragPosition);

		float specularFactor = dot(R, E);

		if (specularFactor > 0)
		{
			vec4 materialSpecularColor = mat.specularColor * texture(specularMap, fragTexCoord);
			specularColor = light.color * materialSpecularColor * mat.ks * pow(specularFactor, mat.shininess);
		}
						
		float attenuation = light.attenuation.constant + 
				light.attenuation.linear * distanceToPoint +
				light.attenuation.exponential * distanceToPoint * distanceToPoint + 
				0.0001;

		fragColor = (diffuseColor + specularColor) / attenuation;
	}

	//fragColor = vec4(fragNormal, 1.0);
}