/*

#version 330

in vec2 fragTexCoord;

uniform sampler2D colorMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D reflectivityMap;
uniform vec2 res;
uniform mat4 p;

out vec4 fragColor;

float n = 0.1;
float f = 10.0;
int steps = 50;
int binarySteps = 10;
float rayStep = 0.1;

float getReflectivity(vec2 uv)
{
	return texture(reflectivityMap, uv).r;
}

vec4 getColor(vec2 uv)
{
	return texture(colorMap, uv);
}

vec3 getPosition(vec2 uv)
{
	return texture(positionMap,uv).xyz;
}

vec3 getNormal(vec2 uv)
{
	return texture(normalMap,uv).xyz;
}

float getDepth(vec2 uv)
{
	return texture(depthMap,uv).x;
}

vec4 projectToScreen(vec3 position)
{
	vec4 projectedCoord = p * vec4(position, 1.0);

    projectedCoord.xyz /= projectedCoord.w;
    projectedCoord.xyz *= 0.5;
    projectedCoord.xyz += 0.5;

    return projectedCoord;
}

vec4 binaryMarch(vec3 origin, vec3 direction, float dist) 
{
	float stepDist = dist * 0.5;
	float curDist = -stepDist;

	vec3 pos = vec3(0.0);

	for (int i = 0; i < binarySteps; i++)
	{
		pos = origin + direction * curDist;

		vec4 projPos = projectToScreen(pos);

		float depth = texture(positionMap, projPos.xy).z;

		stepDist *= 0.5;

		if (pos.z - depth > 0.0)
		{
			curDist += stepDist;
		}
		else
		{
			curDist -= stepDist;
		}
	}

	pos = origin + direction * curDist;
	vec2 uv = projectToScreen(pos).xy;

	return getColor(uv);
}


vec4 march(vec3 origin, vec3 direction, float dist) 
{
	float curDist = dist;
	vec3 pos = vec3(0.0);

	for (int i = 0; i < steps; i++)
	{
		pos = origin + direction * curDist;

		vec4 projPos = projectToScreen(pos);

		if (projPos.x < 0.0 || projPos.x > 1.0 || projPos.y < 0.0 || projPos.y > 1.0 || projPos.z > 1.0 || projPos.z < 0.0)
            return vec4(0.0);

		float depth = texture(positionMap, projPos.xy).z;

		if (pos.z - depth < 0.0)
		{
			return binaryMarch(pos, direction, dist);
		}
		else
		{
			curDist += dist;
		}
	}
    
    return vec4(0.0);
}

void main(void)
{
	vec3 position = getPosition(fragTexCoord); // cam coords
	vec3 normal = getNormal(fragTexCoord); // cam coords
	vec3 eye = vec3(0.0, 0.0, 0.1);
	vec3 dir = normalize(position - eye);
	vec3 ref = normalize(reflect(dir, normal));

	vec4 color = getColor(fragTexCoord);
	vec4 refColor = vec4(0.0);

	//if (dot(ref, dir) > 0.0)
		refColor = march(position, ref, rayStep);
	//else
	//	refColor = vec4(1.0, 1.0, 0.0, 1.0);

	fragColor = mix(color, refColor, getReflectivity(fragTexCoord)) ;

	//fragColor = getColor(fragTexCoord);
}
*/

#version 330
in vec2 fragTexCoord;

uniform sampler2D colorMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D reflectivityMap;
uniform vec2 res;
uniform mat4 p;

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

vec4 ComputeReflection()
{
	//Tweakable variables
	float initialStepAmount = .01;
	float stepRefinementAmount = 0.7;
	int maxRefinements = 3;
	
	//Values from textures
	vec2 screenSpacePosition2D = getScreenSpacePosition();
	vec3 cameraSpacePosition = texture(positionMap,screenSpacePosition2D).xyz;
	vec3 cameraSpaceNormal = texture(normalMap,screenSpacePosition2D).xyz;
	//float roughness = texture(otherTexture,screenSpacePosition2D).x;
	//float reflectivity = texture(otherTexture,screenSpacePosition2D).y;

	//Screen space vector
	vec3 cameraSpaceViewDir = normalize(cameraSpacePosition);
	vec3 cameraSpaceVector = normalize(reflect(cameraSpaceViewDir,cameraSpaceNormal));
	vec3 screenSpacePosition = convertCameraSpaceToScreenSpace(cameraSpacePosition);
	vec3 cameraSpaceVectorPosition = cameraSpacePosition + cameraSpaceVector;
	vec3 screenSpaceVectorPosition = convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);
	vec3 screenSpaceVector = initialStepAmount*normalize(screenSpaceVectorPosition - screenSpacePosition);
	
	//Jitter the initial ray
	//float randomOffset1 = clamp(rand(gl_FragCoord.xy),0,1)/1000.0;
	//float randomOffset2 = clamp(rand(gl_FragCoord.yy),0,1)/1000.0;
	//screenSpaceVector += vec3(randomOffset1,randomOffset2,0);
	vec3 oldPosition = screenSpacePosition + screenSpaceVector;
	vec3 currentPosition = oldPosition + screenSpaceVector;
	
	//State
	vec4 color = texture(colorMap, fragTexCoord);
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
		float sampleDepth = linearizeDepth(texture(depthMap, samplePos).x);
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
				vec3 normalAtPos = texture(normalMap, samplePos).xyz;
				float orientation = dot(cameraSpaceVector,normalAtPos);

				if(orientation < 0)
				{
					
					color = texture(colorMap, samplePos) * cosAngIncidence;
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
	fragColor =  mix(texture(colorMap, fragTexCoord), ComputeReflection(), texture(reflectivityMap, fragTexCoord).r);
	//fragColor = texture(colorMap, screenSpacePosition);
	//fragColor = texture(reflectivityMap, fragTexCoord);

	float reflectivity = texture(reflectivityMap, fragTexCoord).r;

	if(reflectivity > 0.01) //Don't draw background or non reflective pixels
	{
		fragColor = reflectivity * ComputeReflection() + (1.0 - reflectivity) * fragColor;
	}


	//if (fragTexCoord.x < 0.5)
	//   	fragColor = texture(positionMap, fragTexCoord);
	//else
	//   	fragColor = texture(normalMap, fragTexCoord);

	//fragColor = ComputeReflection();
	
}
/*
#version 330
in vec2 fragfragTexCoord;

uniform sampler2D colorMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D reflectivityMap;
uniform vec2 res;
uniform mat4 p;

out vec4 fragColor;

float n = 0.1;
float f = 1000.0;
float rayStepSize = 0.5;

float linearizeDepth(float depth)
{
	return (2.0 * n) / (f + n - depth * (f - n));
}

vec3 raytrace(in vec3 reflectionVector, in float startDepth)
{
	vec3 color = vec3(0.0);
	float stepSize = rayStepSize; 
 
	float size = length(reflectionVector.xy);
	reflectionVector = normalize(reflectionVector/size);
	reflectionVector = reflectionVector * stepSize;
        
        // Current sampling position is at current fragment
	vec2 sampledPosition = fragTexCoord;
        // Current depth at current fragment
	float currentDepth = startDepth;
        // The sampled depth at the current sampling position
	float sampledDepth = linearizeDepth( texture(depthMap, sampledPosition).z );
 
        // Raytrace as long as in texture space of depth buffer (between 0 and 1)
	while(sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 &&
	      sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0)
	{
                // Update sampling position by adding reflection vector's xy and y components
		sampledPosition = sampledPosition + reflectionVector.xy;
                // Updating depth values
		currentDepth = currentDepth + reflectionVector.z * startDepth;
		float sampledDepth = linearizeDepth( texture(depthMap, sampledPosition).z );
                
                // If current depth is greater than sampled depth of depth buffer, intersection is found
		if(currentDepth > sampledDepth)
		{
                        // Delta is for stop the raytracing after the first intersection is found
                        // Not using delta will create "repeating artifacts"
			float delta = (currentDepth - sampledDepth);
			if(delta < 0.003 )
			{
				color = texture(colorMap, sampledPosition).rgb;
				break;
			}
		}
	}
 
	return color;
}

vec4 SSR()
{
	vec3 reflectedColor = vec3(0.0);
 
	vec3 normal = normalize( texture(normalMap, fragTexCoord) ).xyz;
	vec3 position = texture(positionMap, fragTexCoord).xyz;
  
	// Depth at current fragment
	float currDepth = linearizeDepth( texture(depthMap, fragTexCoord).z );
 
	// Eye position, camera is at (0, 0, 0), we look along negative z, add near plane to correct parallax
	vec3 eyePosition = normalize( vec3(0, 0, n) );
	//vec4 reflectionVector = p * reflect(vec4(-eyePosition, 0), vec4(normal, 0) ) ;
	vec4 reflectionVector = p * vec4(reflect(position, normal), 0);
 
        // Call raytrace to get reflected color
	reflectedColor = raytrace(reflectionVector.xyz, currDepth);	
 
 
	return vec4(reflectedColor, 1.0);
}

void main()
{
	fragColor = mix(texture(colorMap, fragTexCoord), SSR(), texture(reflectivityMap, fragTexCoord));
}
*/