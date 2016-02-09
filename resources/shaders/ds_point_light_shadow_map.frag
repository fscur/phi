#version 330

in vec3 fragWorldPos;
in vec3 fragLightDir;

uniform float nearPlane;
uniform float farPlane;
uniform vec3 lightPos;

out vec2 fragColor;

vec2 varianceShadow()
{
	//float depth = gl_FragCoord.z;

	float depth = length(fragLightDir);
	depth /= (farPlane - nearPlane);
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);
    return vec2(depth, bias);
    //return vec2(depth,depth); 
}

void main()
{
	fragColor = varianceShadow();
	//fragColor = vec2(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z) ;
	//float n = nearPlane;
	//float f = farPlane;
	//float c = (2.0 * n) / (f + n - gl_FragCoord.z * (f - n));

	//fragColor = vec2(c, c*c);

    //float dist = length(fragLightDir);

    //fragColor = vec2(dist, dist * dist);
}