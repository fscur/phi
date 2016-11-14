#version 450

struct glyphRenderData
{
    vec4 color;
    vec2 pos;
    vec2 size;
    vec2 texelSize;
    float shift;
    float page;
    int unit;
    int parentModelMatrixIndex;
    int pad3;
    int pad4;
};

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

layout (std140, binding = 1) buffer GlyphRenderData
{
    glyphRenderData items[];
} glyphs;


layout (std140, binding = 2) buffer ParentModelMatricesData
{
    mat4 items[];
} parentModelMatrices;

out vec2 fragTexCoord;
out vec3 fragPosition;

flat out int unit;
flat out float page;
flat out float shift;
flat out vec4 color;
flat out vec2 texelSize;

void main()
{
    glyphRenderData renderData = glyphs.items[gl_InstanceID];
    gl_Position = frameUniforms.p * frameUniforms.v * parentModelMatrices.items[renderData.parentModelMatrixIndex] * inModelMatrix * vec4(inPosition, 1.0);
    fragTexCoord = renderData.pos + inTexCoord * renderData.size; //TODO: ver se da pra melhorar matematicamente plausivel
    fragPosition = inPosition;
    unit = renderData.unit;
    page = renderData.page;
    shift = renderData.shift;
    color = renderData.color;
    texelSize = renderData.texelSize;
}
