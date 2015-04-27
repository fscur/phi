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

struct pointLight
{
	vec3 position;
	vec4 color;
	float intensity;
	float range;
	float oneOverRangeSqr;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;
in vec3 fragWorldPos;

uniform mat4 v;
uniform mat4 m;

uniform pointLight light;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform samplerCube shadowMap;

out vec4 fragColor;

float linstep(float value, float low, float high)
{
	return clamp((value-low)/(high-low), 0.0, 1.0);
}

float calcVarianceShadowFactor()
{	
	vec3 lightDir = fragWorldPos - light.position;
	float distanceToLight = length(lightDir);

	vec2 moments = texture(shadowMap, lightDir).xy;

	if (distanceToLight <= moments.x)
		return 1.0;

	float p = step(distanceToLight, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);

	float d = distanceToLight - moments.x;
	float pMax = linstep(variance / (variance + d * d), 0.8, 1.0);

	return clamp(min(max(p, pMax), 1.0), 0.2, 1.0);
}

float calcShadowFactor()
{
	vec3 lightDir = fragWorldPos - light.position;
	float distanceToLight = length(lightDir);

	float depth = texture(shadowMap, lightDir).x;

	if (depth < distanceToLight - 0.1)
		return 0.2;
	else
		return 1.0;
}

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
			
		float attenuation = 1 - pow(distanceToPoint, 2.0) * light.oneOverRangeSqr;

		vec4 emissiveColorMap = texture(emissiveMap, fragTexCoord.xy);
		fragColor = (diffuseColor + specularColor) * attenuation;

		float shadowFactor = calcShadowFactor();

		fragColor = vec4(fragColor.rgb * shadowFactor, mat.isEmissive * emissiveColorMap.r);

		//fragColor  = vec4(vec3(shadowFactor), 1.0);
	}

	//fragColor = vec4(fragLight, 1.0);
}