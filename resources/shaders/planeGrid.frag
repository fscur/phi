#version 450
#extension GL_ARB_bindless_texture: enable

struct planeGridRenderData
{
    vec4 color;
    int textureUnit;
    int pad0;
    int pad1;
    int pad2;
    float texturePage;
    float pad3;
    float pad4;
    float pad5;
};

layout (std140, binding = 1) buffer PlaneGridRenderDataBuffer
{
    planeGridRenderData items[];
} renderData;

layout (location = 0) uniform sampler2DArray textureArrays[32];

//layout (location = 1) uniform float radiusFadeIn;
//layout (location = 2) uniform float radiusFadeOut;
//layout (location = 3) uniform float radiusWave;
//layout (location = 4) uniform vec2 expansionPosition;
//layout (location = 5) uniform int textureArrayIndex;
//layout (location = 6) uniform float texturePageIndex;
//layout (location = 7) uniform vec4 color;
//layout (location = 8) uniform sampler2DArray textureArrays[32];

in vec2 fragTexCoord;
in vec2 planeCenter;

flat in uint instanceId;
flat in float time;

layout (location = 0) out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
//const float CELL_SIZE = 1.0;
//const float FADE_IN_DISTANCE = 5.0;
//const float FADE_OUT_DISTANCE = 10.0;

vec4 fetch(in vec2 uv)
{
    planeGridRenderData data = renderData.items[instanceId];
    int array = data.textureUnit;
    float page = data.texturePage;
    
    vec4 textureColor = texture(textureArrays[array], vec3(uv, page));
    return textureColor;
}

float contour(in float dist, in float w) 
{
    return smoothstep(0.5 - w, 0.5 + w, dist);
    //return smoothstep(0.7 - w, 0.7 + w, dist-0.04);
}

float samp(in float dist, in float w) 
{
    return contour(dist, w);
}

float sdf(vec2 uv)
{
    float stripes = 2.0;
    float thickness = 40.0;
    float sharpness = 1.0;
    vec2 a = sin(stripes * 0.5 * PI * uv);
    vec2 b = abs(a);
    
    float sdf = 0.0;
    sdf += sharpness * exp(-thickness * b.x);
    sdf += sharpness * exp(-thickness * b.y);
    return sdf.r;
}

float applyEdgeAA(in vec2 uv, in float alpha, in float width)
{
    //// Supersample, 4 extra points
    float dscale = 0.354; // half of 1/sqrt2; you can play with this
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv-duv, uv+duv);

    float asum = samp(sdf(box.xy).r, width)
               + samp(sdf(box.zw).r, width)
               + samp(sdf(box.xw).r, width)
               + samp(sdf(box.zy).r, width);

    // weighted average, with 4 extra points having 0.5 weight each,
    // so 1 + 0.5*4 = 3 is the divisor
    return (alpha + 0.5 * asum) / 3.0;
}

void main()
{
    //////////////////////////////////////// ripples nandinho
    //vec2 diff = fragTexCoord - expansionPosition;
    //float dist = length(diff);

    //float x = radiusWave - dist; // How close to the wave radius the current point is from the expansion position.

    //float res = (sin(x * PI) / x) * 0.25;
    //if (abs(res) < 0.01)
    //    res = 0.0;

    //vec2 uv = fragTexCoord * (1.0 / CELL_SIZE) + normalize(diff) * res + vec2(0.5);

    //vec4 texColor = texture(textureArrays[textureArrayIndex], vec3(uv, texturePageIndex));
    //float alphaFadeIn = 1.0 - clamp(abs(min(dist - radiusFadeIn, 0.0)) / FADE_IN_DISTANCE, 0.0, 1.0);
    //float alphaFadeOut = clamp((radiusFadeOut - dist) / FADE_OUT_DISTANCE, 0.0, 1.0);

    //float alpha = texColor.a * min(alphaFadeIn, alphaFadeOut);
    //alpha += alpha * res * 0.3;

    //fragColor = vec4(texColor.rgb, alpha) * color;




    //////////////////////////////////////// ripples

    //float dist = length(uv);
    //float rippleRadius = time;
    //float diff = rippleRadius - dist;
    //float func = sin(PI * diff);
    //uv += uv * func * 0.1;




    ////////////////////////////////////plane rendering
    //sdf from texture

    //planeGridRenderData data = renderData.items[instanceId];

    //float dist = fetch(fragTexCoord).r;
    //float width = fwidth(dist);
    //float alpha = contour(dist, width);
    //alpha = applyEdgeAA(alpha, width);
    //alpha += 0.5;
    
    //float d = length(planeCenter + 0.5);
    //float f = 1.0 - smoothstep(0.0, 5.5, d);

    //fragColor = vec4(data.color.rgb, alpha * f);




    //////////////////////////////////////////plane rendering
    //sdf from math function

    planeGridRenderData data = renderData.items[instanceId];
    vec2 uv = fragTexCoord+0.5;
    float dist = sdf(uv);
    float width = fwidth(dist);
    float alpha = contour(dist, width);
    alpha = applyEdgeAA(uv, alpha, width);
    alpha += 0.5;
    
    float d = length(planeCenter+0.5);
    float f = 1.0 - smoothstep(0.0, 4.5, d);

    fragColor = vec4(data.color.rgb, alpha * f);
}