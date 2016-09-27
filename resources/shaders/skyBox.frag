#version 450
#extension GL_ARB_bindless_texture: enable

struct skyBoxRenderData
{
    vec4 color;
};

layout (std140, binding = 1) buffer SkyBoxRenderDataBuffer
{
    skyBoxRenderData items[];
} renderData;

//layout (location = 0) uniform sampler2DArray textureArrays[32];

in vec2 fragTexCoord;
in vec3 fragPosCoord;
flat in uint instanceId;

layout (location = 0) out vec4 fragColor;

//vec4 fetch(vec2 uv)
//{
//    controlRenderData data = renderData.items[instanceId];
//    int array = data.backgroundTextureUnit;
//    float page = data.backgroundTexturePage;
    
//    vec4 textureColor = texture(textureArrays[array], vec3(uv, page));
//    return textureColor * data.backgroundColor;
//}

void main(void)
{
    skyBoxRenderData data = renderData.items[instanceId];
    fragColor = vec4(data.color.rgb * fragPosCoord.yxz + 0.5f, 1.0);
}
