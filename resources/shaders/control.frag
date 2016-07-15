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

layout (std140, binding = 1) buffer ControlRenderDataBuffer
{
    controlRenderData items[];
} renderData;

layout (location = 0) uniform sampler2DArray textureArrays[32];

in vec2 fragTexCoord;
flat in uint instanceId;

layout (location = 0) out vec4 fragColor;

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
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    fragColor = fetch(fragTexCoord);
    //fragColor = vec4(1.0);
    //fragColor = vec4(instanceId);
}
