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

struct spotLight
{
	vec3 position;
	vec4 color;
	float intensity;
	float range;
	float oneOverRangeSqr;
	vec3 direction;
	float cutoff;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragLight;
in vec3 fragEye;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform mat4 v;
uniform mat4 m;

uniform spotLight light;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

out vec4 fragColor;

void main()
{
	fragColor = vec4(0.0);

	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;
	
	vec3 lightDir = fragPosition - (v * vec4(light.position, 1.0)).xyz;

	vec3 spotDir = normalize(mat3(v) * light.direction);
	float spotFactor = dot(spotDir, normalize(lightDir));
	float cutoff = light.cutoff;

	if (spotFactor > cutoff)
	{
		float distanceToPoint = length(lightDir);
	
		if (light.range < distanceToPoint)
		{
			//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
			return;
		}

		lightDir = normalize(lightDir);

		float diffuseFactor = dot(normal, -lightDir);

		vec4 diffuseColor =  vec4(0.0);
		vec4 specularColor = vec4(0.0);

		//fragColor = vec4(1.0, 0.0, 0.0, 1.0);

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

			float attenuation = 1 - pow(distanceToPoint, 2.0) * light.oneOverRangeSqr;

			float fadeEdgeFactor = 1.0 - ((1.0 - spotFactor)/(1.0 - light.cutoff + 0.0001));

			fragColor = (diffuseColor + specularColor) * attenuation;
			fragColor *= fadeEdgeFactor;
		}
	}

	//fragColor = vec4(1.0);
}