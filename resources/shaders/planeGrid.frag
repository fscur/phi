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

layout (location = 0) out vec4 fragColor;

//const float PI = 3.1415926535897932384626433832795;
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

float contour(in float d, in float w) {
    // smoothstep(lower edge0, upper edge1, x)
    return smoothstep(0.7 - w, 0.7 + w, d-0.04);
}

float samp(in vec2 uv, in float w) {
    return contour(fetch(uv).r, w);
}

void main()
{
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

    //fragColor = fetch(fragTexCoord);

    planeGridRenderData data = renderData.items[instanceId];
    int array = data.textureUnit;
    float page = data.texturePage;
    
    vec4 textureColor = texture(textureArrays[array], vec3(fragTexCoord, page));
    //fragColor = textureColor;

    float dist = textureColor.r;

    // fwidth helps keep outlines a constant width irrespective of scaling
    float width = fwidth(dist);
    ////float width = 0.0;
    
    float alpha = contour( dist, width );
    ////float alpha = aastep( 0.5, dist );

    //// ------- (comment this block out to get your original behavior)
    //// Supersample, 4 extra points
    float dscale = 0.354; // half of 1/sqrt2; you can play with this
    vec2 duv = dscale * (dFdx(fragTexCoord) + dFdy(fragTexCoord));
    vec4 box = vec4(fragTexCoord-duv, fragTexCoord+duv);

    float asum = samp( box.xy, width )
               + samp( box.zw, width )
               + samp( box.xw, width )
               + samp( box.zy, width );

    // weighted average, with 4 extra points having 0.5 weight each,
    // so 1 + 0.5*4 = 3 is the divisor
    alpha = (alpha + 0.5 * asum) / 3.0 + 0.1;

    float d = length(planeCenter - 0.5);
    float f = 1.0 - smoothstep(0.0, 0.5, d);

    fragColor = vec4(vec3(0.6, 0.8, 1.0), alpha * f);
}