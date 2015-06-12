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
in vec2 fragTexCoord;
in vec3 fragTangent;
in vec4 fragLightSpacePos;

uniform mat4 v;
uniform mat4 m;

uniform spotLight light;
uniform material mat;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform sampler2D shadowMap;
uniform sampler2D randomNormalMap;

out vec4 fragColor;

vec2 getRandomNormalTexel(vec2 uv)
{
	return normalize(texture(randomNormalMap, uv).xy);
}

float calcShadowFactor(vec3 normal)
{
	float shadowFactor = 0.0;

	vec3 pos = fragLightSpacePos.xyz / fragLightSpacePos.w;
	vec2 uv = pos.xy;

	float texRatio = 1.0/4096.0;

	vec2 vecs[16] = vec2[](
		vec2(1.0, 0.0) * texRatio * 0.25, 
		vec2(-1.0, 0.0) * texRatio * 0.25,
		vec2(0.0 ,1.0) * texRatio * 0.25,
		vec2(0.0, -1.0) * texRatio * 0.25,
		vec2(1.0, 0.0) * texRatio * 0.5, 
		vec2(-1.0, 0.0) * texRatio * 0.5,
		vec2(0.0 ,1.0) * texRatio * 0.5,
		vec2(0.0, -1.0) * texRatio * 0.5,
		vec2(1.0, 0.0) * texRatio * 0.75, 
		vec2(-1.0, 0.0) * texRatio * 0.75,
		vec2(0.0 ,1.0) * texRatio * 0.75,
		vec2(0.0, -1.0) * texRatio * 0.75,
		vec2(1.0, 0.0) * texRatio, 
		vec2(-1.0, 0.0) * texRatio,
		vec2(0.0 ,1.0) * texRatio,
		vec2(0.0, -1.0) * texRatio);

	int iterations = 16;

	vec2 random = getRandomNormalTexel(fragTexCoord);
	float cosTheta = dot(normal, normalize(pos));
	float bias = 0.005 * tan(acos(cosTheta));
	bias = clamp(bias, 0, 0.000001);

	for (int j = 0; j < iterations; ++j)
	{
		vec2 offset = reflect(vecs[j], random);
		//vec2 offset = reflect(vecs[j], random) * texRatio;
		float depth = texture(shadowMap, uv + offset).r;

		if (depth < (pos.z - bias))
			shadowFactor += 0.8;
	}

	shadowFactor /= iterations;

	return 1.0 - shadowFactor;
}

float linstep(float value, float low, float high)
{
	return clamp((value-low)/(high-low), 0.0, 1.0);
}

float calcVarianceShadowFactor()
{	
	vec3 pos = fragLightSpacePos.xyz / fragLightSpacePos.w;
	vec2 uv = pos.xy;

	vec2 moments = texture(shadowMap, uv).xy;

	if (pos.z <= moments.x)
		return 1.0;

	float p = step(pos.z, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);

	float d = pos.z - moments.x;
	float pMax = linstep(variance / (variance + d * d), 0.8, 1.0);

	return clamp(min(max(p, pMax), 1.0), 0.2, 1.0);
}

float calcExponentShadowFactor(vec3 normal)
{
    vec3 pos = fragLightSpacePos.xyz / fragLightSpacePos.w;
	float occluder = texture(shadowMap, pos.xy).r;
    float receiver = length((v * vec4(light.position, 1.0)).xyz - fragPosition.xyz);
    receiver = clamp((receiver - 0.1) / (light.range - 0.1), 0.0, 1.0);

    float overdark = 1.0;
    float lit = clamp(exp((overdark * (occluder-receiver))), 0.0, 1.0);

    return clamp(lit, 0.5, 1.0);
}

void main()
{
	fragColor = vec4(0.0);

	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;
	
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
		float diffuse = light.intensity * max(0.0, dot(normal, s)) * mat.kd;
		float spec = pow(max(0.0, dot(normal,h)), mat.shininess) * mat.ks;

		vec4 diffuseColorMap = texture(diffuseMap, fragTexCoord);
		vec4 specularColorMap = texture(specularMap, fragTexCoord);
		vec4 emissiveColorMap = texture(emissiveMap, fragTexCoord.xy);

		vec4 diffuseComponent = diffuseColorMap * diffuse * mat.diffuseColor * light.color;
		vec4 specularComponent = specularColorMap * spec * mat.specularColor * light.color;

		float shadowFactor = calcVarianceShadowFactor();

		vec4 color = (diffuseComponent + specularComponent);

		float attenuation = 1 - pow(distanceToPoint, 2.0) * light.oneOverRangeSqr;

		color *= attenuation;

		float fadeEdgeFactor = 1.0 - ((1.0 - spotFactor)/(1.0 - light.cutoff + 0.0001));

		color *= fadeEdgeFactor;

		fragColor = vec4(color.rgb * shadowFactor, mat.isEmissive * emissiveColorMap.r);
	}
}