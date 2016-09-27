#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsDataBuffer
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

out vec2 fragTexCoord;
out vec3 fragPosCoord;
flat out uint instanceId;

void main()
{
    vec3 xAxis = normalize(frameUniforms.v * vec4(1.0, 0.0, 0.0, 0.0)).xyz;
    vec3 yAxis = normalize(frameUniforms.v * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
    vec3 zAxis = normalize(frameUniforms.v * vec4(0.0, 0.0, 1.0, 0.0)).xyz;

    mat3 rotationMatrix = mat3(xAxis, yAxis, zAxis);

    gl_Position = frameUniforms.p * mat4(rotationMatrix) * vec4(inPosition * 100.0, 1.0);

    fragTexCoord = inTexCoord;
    fragPosCoord = inPosition;
    instanceId = gl_InstanceID;
}