#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;

layout (location = 0) uniform int unit;
layout (location = 1) uniform float page;
layout (location = 2) uniform vec2 resolution;
layout (location = 3) uniform sampler2DArray textureArrays[32];

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

layout (location = 0) out vec4 fragColor;

vec3 fetch(vec2 uv)
{
    return texture(textureArrays[unit], vec3(uv, page)).rgb;
}

void main() 
{
    vec2 uv = fragTexCoord.xy;
    vec3 color = vec3(0.0, 0.0, 0.0);
    color = fetch(uv) * weight[0];

    for (int i = 1; i < 3; i++)
    {
        vec2 displacement = vec2(offset[i] / resolution.x, 0.0);
        color += fetch(uv + displacement) * weight[i];
        color += fetch(uv - displacement) * weight[i];
    }

    fragColor = vec4(color, 1.0);
}