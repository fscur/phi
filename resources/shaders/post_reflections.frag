#version 330
in vec2 fragTexCoord;

uniform sampler2D rt0;
uniform sampler2D rt1;
uniform sampler2D rt2;
uniform sampler2D rt3;
uniform sampler2D rt4;
uniform vec2 res;
uniform mat4 ip;
uniform mat4 p;

out vec4 fragColor;

float n = 0.1;
float f = 1000.0;
int steps = 50;
int binarySteps = 10;
float rayStep = 0.1;

vec3 decodePosition(vec2 texCoord)
{
	vec4 clipSpaceLocation;
	clipSpaceLocation.xy = texCoord * 2.0 - 1.0;
	clipSpaceLocation.z = texture(rt4, texCoord).r * 2.0 - 1.0;
	clipSpaceLocation.w = 1.0;

	vec4 homogenousLocation = ip *  clipSpaceLocation;
	return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 decodeNormal (vec2 texCoord)
{
   	vec4 c1 = texture(rt1, texCoord);
   	vec4 c2 = texture(rt2, texCoord);

   	vec2 enc = vec2(c1.w, c2.w);

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

//Random function used for jittering rays
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

//Z buffer is nonlinear by default, so we fix this here
float linearizeDepth(float depth)
{
	return (2.0 * n) / (f + n - depth * (f - n));
}

vec2 getScreenSpacePosition()
{
	return fragTexCoord;
}

//Convert something in camera space to screen space
vec3 convertCameraSpaceToScreenSpace(in vec3 cameraSpace)
{
	vec4 clipSpace = p * vec4(cameraSpace, 1);
	vec3 NDCSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5 * NDCSpace + 0.5;
	return screenSpace;
}

vec4 ComputeReflection(vec4 color)
{
	//Tweakable variables
	float initialStepAmount = .01;
	float stepRefinementAmount = 0.7;
	int maxRefinements = 3;
	
	//Values from textures
	vec3 cameraSpacePosition = decodePosition(fragTexCoord).xyz;
	vec3 cameraSpaceNormal = decodeNormal(fragTexCoord);

	//Screen space vector
	vec3 cameraSpaceViewDir = normalize(cameraSpacePosition);
	vec3 cameraSpaceVector = normalize(reflect(cameraSpaceViewDir,cameraSpaceNormal));
	vec3 screenSpacePosition = convertCameraSpaceToScreenSpace(cameraSpacePosition);
	vec3 cameraSpaceVectorPosition = cameraSpacePosition + cameraSpaceVector;
	vec3 screenSpaceVectorPosition = convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);
	vec3 screenSpaceVector = initialStepAmount * normalize(screenSpaceVectorPosition - screenSpacePosition);
	
	vec3 oldPosition = screenSpacePosition + screenSpaceVector;
	vec3 currentPosition = oldPosition + screenSpaceVector;
	
	//State
	int count = 0;
	int numRefinements = 0;

	while(count < 100)
	{
		//Stop ray trace when it goes outside screen space
		if(currentPosition.x < 0 || currentPosition.x > 1 ||
		   currentPosition.y < 0 || currentPosition.y > 1 ||
		   currentPosition.z < 0 || currentPosition.z > 1)
			break;

		//intersections
		vec2 samplePos = currentPosition.xy;
		float currentDepth = linearizeDepth(currentPosition.z);
		float sampleDepth = linearizeDepth(texture(rt4, samplePos).r);
		float diff = currentDepth - sampleDepth;
		//float error = length(screenSpaceVector);
		float error = 0.005;

		float cosAngIncidence = -dot(cameraSpaceViewDir,cameraSpaceNormal);
		cosAngIncidence = clamp(1 - cosAngIncidence, 0.0, 1.0);

		//color = vec4(0.0, 0.0, 1.0, 1.0) * cosAngIncidence;
		error *= 1 - cosAngIncidence;

		if(diff >= 0 && diff < error)
		{
			screenSpaceVector *= stepRefinementAmount;
			currentPosition = oldPosition;
			numRefinements++;

			if(numRefinements >= maxRefinements)
			{
				vec3 normalAtPos = decodeNormal(samplePos);
				float orientation = dot(cameraSpaceVector,normalAtPos);

				if(orientation < 0)
				{
					color = texture(rt0, samplePos) * cosAngIncidence;
					//color.a = 1.0;
				}

				break;
			}
		}
		/*
		else if (diff > error)
		{

			color = vec4(1.0, 0.0, 0.0, 1.0);
			break;
		}
		*/

		//Step ray
		oldPosition = currentPosition;
		currentPosition = oldPosition + screenSpaceVector;
		count++;
	}

	return color;
}

//Main
void main()
{
	float reflectivity = texture(rt3, fragTexCoord).x;

	vec4 color = vec4(texture(rt0, fragTexCoord).rgb, 1.0);

	//fragColor = color;
	//fragColor = mix(color, ComputeReflection(color), reflectivity);

	fragColor = color;

	if(reflectivity > 0.01)
	{
		fragColor = reflectivity * ComputeReflection(color) + (1.0 - reflectivity) * fragColor;
	}
}
