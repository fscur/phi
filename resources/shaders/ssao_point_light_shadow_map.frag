#version 330

uniform float nearPlane;
uniform float farPlane;
 
out vec2 fragColor;

vec2 varianceShadow()
{
	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);
    return vec2(depth, bias);
}

void main()
{
	fragColor = varianceShadow();
}