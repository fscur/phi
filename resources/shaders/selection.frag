#version 450

in vec2 fragTexCoord;

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

layout (location = 0) uniform vec2 resolution;
layout (location = 1) uniform float offset;
layout (location = 2) uniform float time;
layout (location = 3) uniform int pickingRTUnit;
layout (location = 4) uniform float pickingRTPage;
layout (location = 5) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

float fetchAlpha(float dx, float dy)
{
    vec2 uv = fragTexCoord + vec2(dx, dy)/resolution;
    vec4 c = texture(textureArrays[pickingRTUnit], vec3(uv.xy, pickingRTPage));
    int selecting = int(c.a * 255.0);
    return (selecting & 1) == 1 ? 1.0 : 0.0;
}

float gradient(float d)
{
    float gx = 0.0;
    gx += -1.0 * fetchAlpha(-d,-d);
    gx += -2.0 * fetchAlpha(-d, 0.0);
    gx += -1.0 * fetchAlpha(-d, d);
    gx +=  1.0 * fetchAlpha( d,-d);
    gx +=  2.0 * fetchAlpha( d, 0.0);
    gx +=  1.0 * fetchAlpha( d, d);

    float gy = 0.0;
    gy += -1.0 * fetchAlpha(-d,-d);
    gy += -2.0 * fetchAlpha( 0.0, -d);
    gy += -1.0 * fetchAlpha( d,-d);
    gy +=  1.0 * fetchAlpha(-d, d);
    gy +=  2.0 * fetchAlpha( 0.0, d);
    gy +=  1.0 * fetchAlpha( d, d);

    return gx*gx + gy*gy;
}

void main()
{
    vec4 edgeColor = vec4(1.0, 1.0, 1.0, 1.0);
    float g = gradient(offset);
    fragColor = edgeColor *  g;
}
