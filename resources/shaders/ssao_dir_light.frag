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
in vec2 fragTexCoord;
in vec3 fragTangent;
in vec4 fragLightSpacePos;

uniform mat4 v;
uniform mat4 m;

uniform material mat;
uniform directionalLight light;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform sampler2D shadowMap;

out vec4 fragColor;

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

float calcShadowFactor()
{
	vec3 pos = fragLightSpacePos.xyz / fragLightSpacePos.w;
	vec2 uv = pos.xy;
	float depth = texture(shadowMap, uv).x;

	if (depth < (pos.z - (1.0/1024)))
		return 0.2;
	else
		return 1.0;
}

vec4 applyFog(vec4 rgb, float distance ) // camera to point distance
{
    float b = 0.005;
    float fogAmount = 1.0 - exp( -distance*b );
    vec4  fogColor  = vec4(0.5,0.6,0.7, 1.0);
    return mix( rgb, fogColor, fogAmount );
}

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
	
	vec4 diffuseComponent = light.color * diffuseColorMap * mat.diffuseColor * diffuseFactor;
	vec4 specularComponent = specularColorMap * mat.specularColor * specFactor;
	
	float shadowFactor = calcVarianceShadowFactor();

	vec4 color = diffuseComponent + specularComponent;

	//color = applyFog(color, abs(fragPosition.z));

	fragColor = vec4(color.rgb * shadowFactor, mat.isEmissive * emissiveColorMap.r);
}

