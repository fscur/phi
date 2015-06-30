#version 330

in vec2 fragTexCoord;

uniform mat4 ip;
uniform vec2 res;
uniform float randomSize;
uniform float sampleRadius;
uniform float intensity;
uniform float scale;
uniform float bias;

uniform sampler2D rt1;
uniform sampler2D rt2;
uniform sampler2D rt3;

uniform sampler2D randomNormalMap;

out vec4 fragColor;

vec3 decodePosition(vec2 texCoord)
{
	vec4 clipSpaceLocation;
	clipSpaceLocation.xy = texCoord * 2.0 - 1.0;
	clipSpaceLocation.z = texture(rt3, texCoord).r * 2.0 - 1.0;
	clipSpaceLocation.w = 1.0;

	vec4 homogenousLocation = ip *  clipSpaceLocation;
	return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 decodeNormal (vec2 enc)
{
    float scale = 1.7777;
    vec3 nn =
        vec3(enc.xy, 0.0) * vec3(2*scale,2*scale,0) +
        vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g*nn.xy;
    n.z = g-1;
    return n;
}

vec2 getRandom(vec2 uv)
{
	vec2 p = res * uv / randomSize;
	return normalize(texture(randomNormalMap, p).xy * 2.0 - 1.0);
}

float doAmbientOcclusion(vec2 tcoord, vec2 uv, vec3 p, vec3 cnorm)
{
	vec3 diff = decodePosition(tcoord + uv) - p;
	vec3 v = normalize(diff);
	float d = length(diff) * scale;
	return max( 0.0, dot(cnorm, v) - bias) * (1.0/(1.0 + d)) * intensity;

	//float d = length(diff) * 1.0;
	//return max( 0.0, dot(cnorm, v) - 0.7) * (1.0/(1.0 + d)) * 5.0;
}

void main(void)
{
	vec4 c1 = texture(rt1, fragTexCoord);
   	vec4 c2 = texture(rt2, fragTexCoord);

	vec3 p = decodePosition(fragTexCoord);
   	vec3 n = decodeNormal(vec2(c1.w, c2.w));
	vec2 random = getRandom(fragTexCoord);

	vec2 vecs[4] = vec2[](
		vec2(1.0, 0.0), 
		vec2(-1.0, 0.0),
		vec2(0.0 ,1.0),
		vec2(0.0, -1.0));

	float ao = 0.0;
	float radius = sampleRadius/p.z;

	fragColor = vec4(1.0);

	int iterations = 4;

	for (int j = 0; j < iterations; ++j)
	{
		vec2 coord1 = reflect(vecs[j], random) * radius;
		vec2 coord2 = vec2(coord1.x*0.7071 - coord1.y*0.7071,
				  coord1.x*0.7071 + coord1.y*0.7071);

		ao += doAmbientOcclusion(fragTexCoord, coord1 * 0.25, p, n);
		ao += doAmbientOcclusion(fragTexCoord, coord2 * 0.5, p, n);
		ao += doAmbientOcclusion(fragTexCoord, coord1 * 0.75, p, n);
		ao += doAmbientOcclusion(fragTexCoord, coord2, p, n);
	}
	
	ao /= (iterations * 4.0);

	fragColor = vec4(1.0) * ao;
	
	/*
	float z = texture(positionMap, fragTexCoord).r;      // fetch the z-value from our depth texture
	float n = 1.0;                                // the near plane
	float f = 20.0;                               // the far plane
	float c = (2.0 * n) / (f + n - z * (f - n));  // convert to linear values 
	 
	fragColor.rgb = vec3(c); 
	*/

	//fragColor = texture(positionMap, fragTexCoord);
	

	//fragColor = texture(positionMap, fragTexCoord);
}