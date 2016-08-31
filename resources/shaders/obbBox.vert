#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in mat4 inModelMatrix;
layout (location = 7) in mat4 inRotationMatrix;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

out flat float time;
out flat vec2 scale;
out flat float offset;
out flat float planeDist;
out flat float planeDist2;

const float PI = 3.1415926535897932384626433832795;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
    vec2 resolution;
    float near;
    float far;
    float halfFovTangent;
    float time; 
    float pad0;
    float pad1;
} frameUniforms;

void main()
{
    vec4 modelPosition =  inModelMatrix * vec4(inPosition, 1.0);
    modelPosition += vec4(0.0, 0.0, 0.0, 0.0);

    vec3 xAxis = normalize(inModelMatrix * vec4(1.0, 0.0, 0.0, 0.0)).xyz;
    vec3 yAxis = normalize(inModelMatrix * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
    vec3 zAxis = normalize(inModelMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;

    mat3 rotationMatrix = mat3(xAxis, yAxis, zAxis);
    vec3 localFaceCenter = inNormal * 0.5f;

    vec3 normal = normalize(rotationMatrix * inNormal);
    vec3 localFacePosition = inPosition - localFaceCenter;
    vec3 position = rotationMatrix * localFacePosition;

    offset = 0.00;
    position *= offset;
    position += normal * 0.000;
    
    mat4 vp = frameUniforms.p * frameUniforms.v;
    gl_Position = vp * vec4(modelPosition.xyz + position, 1.0);
    //gl_Position = vp * vec4(inPosition, 1.0);
    fragPosition = modelPosition.xyz;

    mat4 translationMatrix = 
        mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2], 1.0));

    vec4 xAxis2 = normalize(inModelMatrix * vec4(1.0, 0.0, 0.0, 0.0));
    vec4 yAxis2 = normalize(inModelMatrix * vec4(0.0, 1.0, 0.0, 0.0));
    vec4 zAxis2 = normalize(inModelMatrix * vec4(0.0, 0.0, 1.0, 0.0));
    vec4 wAxis2 = vec4(0.0, 0.0, 0.0, 1.0);

    mat4 rotationMatrix2 = mat4(xAxis2, yAxis2, zAxis2, wAxis2);
    mat4 scaleMatrix = inverse(rotationMatrix2) * inverse(translationMatrix) * inModelMatrix;

    vec2 a = (inRotationMatrix * scaleMatrix * vec4(inPosition, 1.0)).xy;
    vec3 u = vec3(0.0);
    
    if (inNormal == vec3(0.0, 1.0, 0.0))
        u = vec3(0.0, 0.0, -1.0);
    else if (inNormal == vec3(0.0, -1.0, 0.0))
        u = vec3(0.0, 0.0, 1.0);
    else
        u = vec3(0.0, 1.0, 0.0);

    vec3 v = cross(u, inNormal);
    float x = (inRotationMatrix * scaleMatrix * vec4(v, 1.0)).x;
    float y = (inRotationMatrix * scaleMatrix * vec4(u, 1.0)).y;
    scale = vec2(x, y);
    //scale = a.xy;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
    time = frameUniforms.time;

    planeDist = -(frameUniforms.v * inModelMatrix * vec4(vec3(0.0), 1.0)).z;
    planeDist2 = -(frameUniforms.v * inModelMatrix * vec4(inPosition * planeDist * 2.0, 1.0)).z;
}