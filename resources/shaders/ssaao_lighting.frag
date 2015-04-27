float calcVarianceShadowMapFactor(vec3 normal)
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