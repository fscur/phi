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

struct planeGridRenderData
{
    vec4 color;
    vec4 clipPlane0;
    vec4 clipPlane1;
    vec4 clipPlane2;

    float lineThickness;
    float opacity;
    float visibility;
    float pad1;

    float clipPlane0Opacity;
    float clipPlane1Opacity;
    float clipPlane2Opacity;
    float pad2;
};

layout (std140, binding = 1) buffer PlaneGridRenderDataBuffer
{
    planeGridRenderData items[];
} renderData;

out vec4 fragWorldPosition;
out vec3 fragViewPosition;
out float planeDist2;
out vec2 fragWorldTexCoord;
out vec2 fragTexCoord;

flat out vec3 fragViewNormal;
flat out float planeSize;
flat out float planeDist;
flat out uint instanceId;
flat out float globalTime;
flat out vec4 clipPlaneNormal;

vec2 projectPoint(in vec3 point, in vec3 xAxis, in vec3 yAxis)
{
    float x = dot(normalize(xAxis), point);
    float y = dot(normalize(yAxis), point);

    return vec2(x, y);
}

void main()
{
    mat4 modelMatrix = inModelMatrix;
    
    vec3 xAxis = vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]);
    vec3 yAxis = vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]);

    vec3 translation = vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
    vec2 projected = projectPoint(translation, xAxis, yAxis);

    planeDist = -(frameUniforms.v * modelMatrix * vec4(vec3(0.0), 1.0)).z;
    planeSize = planeDist * 2.0;
    planeDist2 = -(frameUniforms.v * modelMatrix * vec4(inPosition * planeSize, 1.0)).z;

    vec4 inPos = vec4(inPosition, 1.0);
    inPos.xy *= planeSize;
    //inPos.xy *= 5.0;
    vec4 pos =  modelMatrix * inPos;

    gl_Position = frameUniforms.p * frameUniforms.v * pos;
    fragViewPosition = -(frameUniforms.v * pos).xyz;
    fragViewNormal = normalize((frameUniforms.v * modelMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz);

    fragWorldTexCoord = ((inTexCoord) * planeSize + projected);
    fragWorldPosition = pos;

    fragTexCoord = (inTexCoord) * planeSize;
    instanceId = gl_InstanceID;

    planeGridRenderData data = renderData.items[gl_InstanceID];

    //gl_ClipDistance[0] = dot(data.clipPlane0, pos);
    //gl_ClipDistance[1] = dot(data.clipPlane1, pos);
    //gl_ClipDistance[2] = dot(data.clipPlane2, pos);
}