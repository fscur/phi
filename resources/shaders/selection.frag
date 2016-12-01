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
layout (location = 1) uniform int pickingRTUnit;
layout (location = 2) uniform float pickingRTPage;
layout (location = 3) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

float rand1(float n){
    return fract(cos(n*89.42)*343.42);
}

float rand2( float n )
{
    return fract(sin(n)*43758.5453);
}

float fetchSelection(float dx, float dy)
{
    vec2 uv = fragTexCoord + vec2(dx, dy)/resolution;
    vec4 c = texture(textureArrays[pickingRTUnit], vec3(uv.xy, pickingRTPage));
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float r = rand1(c.r);
    float g = rand2(c.g);
    float b = rand1(c.b) * rand2(c.b);
    return dot(vec3(r, g, b), luma);
    //return dot(c.rgb, c.rgb);
}

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

float gradient2(float d)
{
    float gx = 0.0;
    gx += -1.0 * fetchSelection(-d,-d);
    gx += -2.0 * fetchSelection(-d, 0.0);
    gx += -1.0 * fetchSelection(-d, d);
    gx +=  1.0 * fetchSelection( d,-d);
    gx +=  2.0 * fetchSelection( d, 0.0);
    gx +=  1.0 * fetchSelection( d, d);

    float gy = 0.0;
    gy += -1.0 * fetchSelection(-d,-d);
    gy += -2.0 * fetchSelection( 0.0, -d);
    gy += -1.0 * fetchSelection( d,-d);
    gy +=  1.0 * fetchSelection(-d, d);
    gy +=  2.0 * fetchSelection( 0.0, d);
    gy +=  1.0 * fetchSelection( d, d);

    return step(0.0001, gx*gx + gy*gy);
}

void main()
{
    vec3 edgeColor = vec3(1.0, 1.0, 1.0);
    vec3 innerEdgeColor = vec3(1.0, 0.5, 0.0);
    vec3 fillColor = vec3(0.75, 0.9, 0.0);
    float g = gradient(1.0);
    float g2 = gradient2(0.75);

    fragColor = 
        vec4(edgeColor, 1.0) * g + 
        vec4(fillColor, 0.3) * fetchAlpha(0.0, 0.0) + 
        vec4(edgeColor, 1.0) * fetchAlpha(0.0, 0.0) * g2;
    
    //fragColor = vec4(vec3(fetchSelection(1.0, 1.0)), 1.0); 

    // vec2 uv = fragTexCoord + vec2(1.0)/resolution;
    // vec4 c = texture(textureArrays[pickingRTUnit], vec3(uv.xy, pickingRTPage));
    // fragColor = vec4(c.rgb, 1.0);
}
