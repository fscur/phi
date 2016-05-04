#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec4 inSelectionColor;
layout (location = 5) in uint inMaterialId;
layout (location = 6) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragTangent;
out flat uint materialId;
out flat vec4 selectionColor;

void main()
{
    mat4 mv = frameUniforms.v * inModelMatrix;
    mat4 mvp = frameUniforms.p * mv;
    mat4 itmv = inverse(transpose(mv));
    gl_Position = mvp * vec4(inPosition, 1.0);
    fragNormal = (itmv * vec4(inNormal, 0.0)).xyz;
    fragTangent = (itmv * vec4(inTangent, 0.0)).xyz;
    fragTexCoord = inTexCoord;
    materialId = inMaterialId;
    selectionColor = inSelectionColor;
}