#version 450

struct glyphInfo
{
    vec4 color;
    vec2 pos;
    vec2 size;
    float shift;
    float page;
    float pad0;
    float pad1;
    int unit;
    int pad2;
    int pad3;
    int pad4;
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in uint inGlyphId;
layout (location = 5) in mat4 inModelMatrix;

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

layout (std140, binding = 1) buffer GlyphInfos
{
    glyphInfo items[];
} glyphs;

out vec2 fragTexCoord;
out vec3 fragPosition;

flat out int unit;
flat out float page;
flat out float shift;
flat out vec4 color;

void main()
{
    gl_Position = frameUniforms.p * frameUniforms.v * inModelMatrix * vec4(inPosition, 1.0);
    glyphInfo info = glyphs.items[inGlyphId];
    fragTexCoord = info.pos + inTexCoord * info.size;
    fragPosition = inPosition;
    unit = info.unit;
    page = info.page;
    shift = info.shift;
    color = info.color;
}
