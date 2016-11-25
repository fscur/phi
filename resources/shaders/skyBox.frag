#version 450
#extension GL_ARB_bindless_texture: enable

struct skyBoxRenderData
{
    int cubeMapUnit;
    int ipad0;
    int ipad1;
    int ipad2;
    float cubeMapPage;
    float fpad0;
    float fpad1;
    float fpad2;
};

layout (std140, binding = 1) buffer SkyBoxRenderDataBuffer
{
    skyBoxRenderData items[];
} renderData;

layout (location = 0) uniform samplerCubeArray cubeMapArrays[5];

in vec2 fragTexCoord;
in vec3 fragPosCoord;
flat in uint instanceId;

layout (location = 0) out vec4 fragColor;

vec4 fetch(vec3 uv)
{
    skyBoxRenderData data = renderData.items[instanceId];
    int array = data.cubeMapUnit;
    float page = data.cubeMapPage;
    
    return texture(cubeMapArrays[array], vec4(uv, page));
}

void main(void)
{
    skyBoxRenderData data = renderData.items[instanceId];
    //fragColor = vec4(data.color.rgb * -fragPosCoord.yxz + 0.5f, 1.0);
    fragColor = fetch(fragPosCoord.xyz);
    //if (fragPosCoord.y < 0.0)
    //fragColor = vec4(1.0);
    //fragColor = vec4(1.0);
}
