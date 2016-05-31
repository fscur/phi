#version 450
#extension GL_ARB_bindless_texture: enable

struct controlRenderData
{
    vec4 backgroundColor;
    int backgroundTextureUnit;
    int pad0;
    int pad1;
    int pad2;
    float backgroundTexturePage;
    float pad3;
    float pad4;
    float pad5;
};

layout (std140, binding = 0) buffer ControlRenderData
{
    controlRenderData items[];
} renderData;

in vec2 fragTexCoord;
flat in uint instanceId;

layout (location = 2) uniform sampler2DArray textureArrays[32];
layout (location = 3) uniform int backgroundUnit;
layout (location = 4) uniform float backgroundPage;
layout (location = 5) uniform vec2 resolution;
layout (location = 6) uniform int level;

out vec4 fragColor;

vec3 fetchGlassyBackground(vec2 uv)
{
    vec3 color = vec3(0.0);
    color += texture(textureArrays[backgroundUnit], vec3(uv, backgroundPage), level + 0).rgb * 0.38774;
    color += texture(textureArrays[backgroundUnit], vec3(uv, backgroundPage), level + 1).rgb * 0.24477 * 2;
    color += texture(textureArrays[backgroundUnit], vec3(uv, backgroundPage), level + 2).rgb * 0.06136 * 2;

    return color;
}

vec4 fetch(vec2 uv)
{
    controlRenderData data = renderData.items[instanceId];
    int array = data.backgroundTextureUnit;
    float page = data.backgroundTexturePage;
    
    vec4 textureColor = texture(textureArrays[array], vec3(uv, page));
    return textureColor * data.backgroundColor;
}

void main(void)
{
    vec3 glassy = fetchGlassyBackground(gl_FragCoord.xy/resolution);
    fragColor = vec4(glassy, 1.0) * vec4(fetch(fragTexCoord).rgb, 1.0);
}