#version 330

struct material
{
	float refractionIndex;
};

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform mat4 p;
uniform vec2 res;

uniform material mat;

uniform sampler2D colorMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D objectNormalMap;

out vec4 fragColor;

float n = 0.1;
float f = 1000.0;
int steps = 50;
int binarySteps = 10;
float rayStep = 0.1;

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

vec4 ComputeRefraction(vec3 normal, vec2 texCoord)
{
	//Tweakable variables
	float initialStepAmount = .001;
	float stepRefinementAmount = 0.7;
	int maxRefinements = 3;
	
	//Values from textures
	vec3 cameraSpacePosition = fragPosition;
	vec3 cameraSpaceNormal = normal;

	//Screen space vector
	vec3 cameraSpaceViewDir = normalize(cameraSpacePosition);
	vec3 cameraSpaceVector = normalize(refract(cameraSpaceViewDir, cameraSpaceNormal, mat.refractionIndex));
	vec3 screenSpacePosition = convertCameraSpaceToScreenSpace(cameraSpacePosition);
	vec3 cameraSpaceVectorPosition = cameraSpacePosition + cameraSpaceVector;
	vec3 screenSpaceVectorPosition = convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);
	vec3 screenSpaceVector = initialStepAmount * normalize(screenSpaceVectorPosition - screenSpacePosition);

	vec3 oldPosition = screenSpacePosition + screenSpaceVector;
	vec3 currentPosition = oldPosition + screenSpaceVector;
	
	//State
	vec4 color = texture(colorMap, texCoord);
	int count = 0;
	int numRefinements = 0;

	while(count < 100)
	{
		//Stop ray trace when it goes outside screen space
		if(currentPosition.x < 0 || currentPosition.x > 1 ||
		   currentPosition.y < 0 || currentPosition.y > 1 ||
		   currentPosition.z < 0 || currentPosition.z > 1)
		{
			//color = vec4(0.0, 1.0, 0.0, 1.0);
			break;
		}

		//intersections
		vec2 samplePos = currentPosition.xy;
		float currentDepth = linearizeDepth(currentPosition.z);
		float sampleDepth = linearizeDepth(texture(depthMap, samplePos).x);
		float diff = currentDepth - sampleDepth;
		float error = 0.005;

		float cosAngIncidence = -dot(cameraSpaceViewDir,cameraSpaceNormal);
		cosAngIncidence = clamp(1 - cosAngIncidence, 0.0, 1.0);

		//error *= 1 - cosAngIncidence;

		if(diff >= 0 && diff < error)
		{
			screenSpaceVector *= stepRefinementAmount;
			currentPosition = oldPosition;
			numRefinements++;

			if(numRefinements >= maxRefinements)
			{
				vec3 normalAtPos = texture(normalMap, samplePos).xyz;
				float orientation = dot(cameraSpaceVector,normalAtPos);

				if(orientation < 0)
				{
					color = texture(colorMap, samplePos) * vec4(0.90, 1.0, 0.90, 1.0);
				}

				break;
			}
		}

		oldPosition = currentPosition;
		currentPosition = oldPosition + screenSpaceVector;
		count++;
	}

	return color;
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy / res;

	vec4 c0 = texture(colorMap, texCoord);
   	vec4 c1 = texture(positionMap, texCoord);
   	vec4 c2 = texture(normalMap, texCoord);
   	vec4 c3 = texture(depthMap, texCoord);

   	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent); 
	vec3 bitangent = cross(normal, tangent);

	mat3 tbn = mat3(tangent, bitangent, normal);

	normal = tbn * (texture(objectNormalMap, fragTexCoord) * 2.0 - 1.0).xyz;

	vec4 refractionColor = ComputeRefraction(normal, texCoord);
	
	if (c1.z > fragPosition.z)
		fragColor = c0;
	else if (mat.refractionIndex > 0)
		fragColor = refractionColor;
	else
		fragColor = c0;
		

	//if (c1.z > fragPosition.z)
	//	fragColor = c0;
	//else 
	//if (mat.refractionIndex > 0)
	//	fragColor = refractionColor;
	//else
//			fragColor = c0;

	//fragColor = vec4(fragPosition, 1.0);		
	/*
	if (mat.refractionIndex > 0.0)
		fragColor = refractionColor;
	else
		fragColor = c0;
*/
	//fragColor = refractionColor * mat.refractionIndex;
}

