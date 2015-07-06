#version 330

struct directionalLight
{
	vec3 position;
	vec4 color;
	float intensity;
	vec3 direction;
};

in vec2 fragTexCoord;

uniform mat4 ip;
uniform mat4 v;
uniform mat4 iv;
uniform mat4 l;

uniform directionalLight light;

uniform sampler2D rt0;
uniform sampler2D rt1;
uniform sampler2D rt2;
uniform sampler2D rt3;
uniform sampler2D shadowMap;

out vec4 fragColor;

float linstep(float value, float low, float high)
{
	return clamp((value-low)/(high-low), 0.0, 1.0);
}

float calcVarianceShadowFactor(vec3 fragPosition)
{	
	vec4 worldSpacePos = iv * vec4(fragPosition, 1.0);
	vec4 lightSpacePos = l * worldSpacePos;
	vec2 uv = lightSpacePos.xy;

	vec2 moments = texture(shadowMap, uv).xy;

	if (lightSpacePos.z <= moments.x)
		return 1.0;

	float p = step(lightSpacePos.z, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);

	float d = lightSpacePos.z - moments.x;
	float pMax = linstep(variance / (variance + d * d), 0.8, 1.0);

	return clamp(min(max(p, pMax), 1.0), 0.2, 1.0);
}

float calcShadowFactor(vec3 fragPosition)
{
	vec4 worldSpacePos = iv * vec4(fragPosition, 1.0);
	vec4 lightSpacePos = l * worldSpacePos;
	vec2 uv = lightSpacePos.xy;
	float depth = texture(shadowMap, uv).x;

	if (depth < (lightSpacePos.z - (1.0/1024)))
		return 0.2;
	else
		return 1.0;
}

vec3 decodePosition(vec2 texCoord)
{
	vec4 clipSpaceLocation;
	clipSpaceLocation.xy = texCoord * 2.0 - 1.0;
	clipSpaceLocation.z = texture(rt3, texCoord).r * 2.0 - 1.0;
	clipSpaceLocation.w = 1.0;

	vec4 homogenousLocation = ip * clipSpaceLocation;
	return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 decodeNormal (vec2 enc)
{
	float scale = 1.7777;
	vec3 nn = vec3(enc.xy, 0.0) * vec3(2*scale, 2*scale,0) + vec3(-scale, -scale, 1);
	float g = 2.0 / dot(nn.xyz,nn.xyz);
	vec3 n;
	n.xy = g*nn.xy;
	n.z = g-1;
	return n;
}

void main(void)
{
	vec4 c0 = texture(rt0, fragTexCoord);
   	vec4 c1 = texture(rt1, fragTexCoord);
   	vec4 c2 = texture(rt2, fragTexCoord);

	vec3 fragPosition = decodePosition(fragTexCoord);
   	float shininess = c0.w * 512;

   	vec3 normal = decodeNormal(vec2(c1.w, c2.w));

   	vec4 diffuseColor = vec4(c1.xyz, 1.0);
   	vec4 specularColor = vec4(c2.xyz, 1.0);

	vec3 lightDir = mat3(v) * normalize(light.direction);

	vec3 s = normalize(-lightDir);
	vec3 fp = normalize(-fragPosition);
	vec3 h = normalize(fp+s);
	float diffuse = light.intensity * max(0.0, dot(normal, s));
	float spec = pow(max(0.0, dot(normal,h)), shininess);

	float shadowFactor = calcShadowFactor(fragPosition);

	fragColor = light.color * diffuseColor * diffuse + light.color * specularColor * spec;
	fragColor *= shadowFactor;
}