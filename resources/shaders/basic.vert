#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in uint inMaterialId;
layout (location = 5) in mat4 inModelMatrix;

layout (std140, binding = 1) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;

}frameUniforms;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragTangent;
flat out uint fragMaterialId;

void main()
{
    mat4 p = frameUniforms.p;
    mat4 v = frameUniforms.v;

    fragPosition = inPosition;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
    fragTangent = inTangent;
    fragMaterialId = inMaterialId;

    gl_Position = p * v * inModelMatrix * vec4(inPosition, 1.0);
}