#version 450
#extension GL_ARB_bindless_texture: enable

struct controlRenderData
{
    vec4 backgroundColor;
    int backgroundTextureUnit;
    int isGlassy;
    int pad1;
    int pad2;
    float backgroundTexturePage;
    float pad3;
    float pad4;
    float pad5;
};

layout (std140, binding = 1) buffer ControlRenderDataBuffer
{
    controlRenderData items[];
} renderData;

layout (std140, binding = 2) uniform GlassyControlUniformBlock
{
    vec2 resolution;
    float backgroundPage;
    float pad0;
    int backgroundUnit;
    int level;
    int pad1;
    int pad2;
} glassyControlUniformBlock;

in vec2 fragTexCoord;
flat in uint instanceId;
flat in vec4 selectionColor;

layout (location = 0) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 pickingColor;

vec3 fetchGlassyBackground(vec2 uv)
{
    float page = glassyControlUniformBlock.backgroundPage;
    int unit = glassyControlUniformBlock.backgroundUnit;
    int level = glassyControlUniformBlock.level;
    //int level = 4;
    vec3 color = vec3(0.0);
    color += texture(textureArrays[unit], vec3(uv, page), level + 0).rgb * 0.38774;
    color += texture(textureArrays[unit], vec3(uv, page), level + 1).rgb * 0.24477 * 2;
    color += texture(textureArrays[unit], vec3(uv, page), level + 2).rgb * 0.06136 * 2;

    return color;
}

vec4 blendFunc(vec4 source, vec4 dest)
{
    return source * source.a + dest * (1.0 - source.a);
}

vec4 fetch(vec2 uv)
{
    controlRenderData data = renderData.items[instanceId];

    int unit = data.backgroundTextureUnit;
    float page = data.backgroundTexturePage;
    
    vec4 textureColor = texture(textureArrays[unit], vec3(uv, page));
    return textureColor;
}

void main(void)
{
    controlRenderData data = renderData.items[instanceId];
    vec4 textureColor = fetch(fragTexCoord);
    if (data.isGlassy == 1)
    {
        vec2 resolution = glassyControlUniformBlock.resolution;
        vec3 glassy = fetchGlassyBackground(gl_FragCoord.xy/resolution);
        fragColor = blendFunc(textureColor, blendFunc(data.backgroundColor, vec4(glassy, 1.0)));
    }
    else
        fragColor = blendFunc(textureColor, data.backgroundColor);

    pickingColor = selectionColor; 
}