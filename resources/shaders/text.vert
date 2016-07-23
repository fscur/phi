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
    int pad2;
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
    float time;
    float pad0;
} frameUniforms;

layout (std140, binding = 1) buffer GlyphRenderData
{
    glyphRenderData items[];
} glyphs;

out vec2 fragTexCoord;
out vec3 fragPosition;

flat out int unit;
flat out float page;
flat out float shift;
flat out vec4 color;
flat out vec2 texelSize;

void main()
{
    gl_Position = frameUniforms.p * frameUniforms.v * inModelMatrix * vec4(inPosition, 1.0);
    glyphRenderData renderData = glyphs.items[gl_InstanceID];
    fragTexCoord = renderData.pos + inTexCoord * renderData.size; //TODO: ver se da pra melhorar matematicamente plausivel
    fragPosition = inPosition;
    unit = renderData.unit;
    page = renderData.page;
    shift = renderData.shift;
    color = renderData.color;
    texelSize = renderData.texelSize;
}
