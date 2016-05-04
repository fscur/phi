#version 450

layout (std140, binding = 0) uniform FrameUniformsBufferData
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
} frameUniforms;

layout (std140, binding = 2) uniform RenderTargetAddresses
{
    int rt0Unit;
    int rt1Unit;
    int rt2Unit;
    int rt3Unit;
    int depthUnit;
    int pad0;
    int pad1;
    int pad2;
    float rt0Page;
    float rt1Page;
    float rt2Page;
    float rt3Page;
    float depthPage;
    float pad3;
    float pad4;
    float pad5;
} rtAddresses;

uniform sampler2DArray textureArrays[32];
uniform vec2 resolution;
uniform float offset;
out vec4 fragColor;
in vec2 fragTexCoord;

float fetchAlpha(float dx, float dy)
{
    vec2 uv = fragTexCoord + vec2(dx, dy)/resolution;
    vec4 c = texture(textureArrays[rtAddresses.rt3Unit], vec3(uv.xy, rtAddresses.rt3Page));
    return c.a;
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
    gy += -2.0 * fetchAlpha( 0.0,   -d);
    gy += -1.0 * fetchAlpha( d,-d);
    gy +=  1.0 * fetchAlpha(-d, d);
    gy +=  2.0 * fetchAlpha( 0.0,    d);
    gy +=  1.0 * fetchAlpha( d, d);

    return gx*gx + gy*gy;
}

void main()
{
    vec4 selectedColor = vec4(0.2, 0.7, 0.6, 1.0);
    vec4 c = texture(textureArrays[rtAddresses.rt3Unit], vec3(fragTexCoord, rtAddresses.rt3Page));

    float g = gradient(offset);
    fragColor = (selectedColor * c.a * 0.5) + selectedColor * g;
}
