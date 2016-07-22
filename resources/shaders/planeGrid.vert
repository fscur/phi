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
} frameUniforms;

out vec2 fragTexCoord;
out vec2 planeCenter;
flat out uint instanceId;

vec2 projectPoint(in vec3 point, in vec3 xAxis, in vec3 yAxis)
{
    float x = dot(normalize(xAxis), point);
    float y = dot(normalize(yAxis), point);

    return vec2(x, y);

    //float dist = dot(point, normal);
    //return point - normal * dist;
}

float planeSize = 10.0;

void main()
{
    mat4 modelMatrix = inModelMatrix;

    //modelMatrix[0][0] = 1.0; modelMatrix[0][1] = 0.0; modelMatrix[0][2] = 0.0; modelMatrix[0][3] = 0.0; 
    //modelMatrix[1][0] = 0.0; modelMatrix[1][1] = 1.0; modelMatrix[1][2] = 0.0; modelMatrix[1][3] = 0.0; 
    //modelMatrix[2][0] = 0.0; modelMatrix[2][1] = 0.0; modelMatrix[2][2] = 1.0; modelMatrix[2][3] = 0.0; 
    //modelMatrix[3][0] = 0.0; modelMatrix[3][1] = 0.0; modelMatrix[3][2] = 0.0; modelMatrix[3][3] = 1.0; 

    vec3 xAxis = vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]);
    vec3 yAxis = vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]);
    ////vec3 zAxis = vec3(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]);

    vec3 translation = vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
    vec2 projected = projectPoint(translation, xAxis, yAxis);

    vec4 inPos = vec4(inPosition, 1.0);
    inPos.xy *= planeSize;
    inPos.xy += 0.5;

    vec4 pos =  modelMatrix * inPos;

    gl_Position = frameUniforms.p * frameUniforms.v * pos;
    //gl_Position.z = 0.0;
    //gl_Position.w = 1.0;
    //gl_Position.z /= gl_Position.w;
    //fragTexCoord = (inTexCoord);
    //fragTexCoord = (inTexCoord - 0.5) * 100 - inPosition.xy;
    fragTexCoord = ((inTexCoord - 0.5) * planeSize + projected);
    planeCenter = (inTexCoord - 0.5) * planeSize;
    instanceId = gl_InstanceID;
}