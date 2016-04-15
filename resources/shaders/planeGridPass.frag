#version 450

uniform float radius;
uniform float radiusHiding;
uniform float scale;
uniform bool showing;
uniform int textureArrayIndex;
uniform float texturePageIndex;
uniform sampler2DArray textureArrays[32];

//layout (std140, binding = 0) uniform FrameUniformsBufferData
//{
//    mat4 p;
//    mat4 v;
//    mat4 vp;
//    mat4 ip;
//} frameUniforms;

in vec2 fragTexCoord;
out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
const float CELL_SIZE = 1.0;

void main()
{
    float r = radius - 7.0;
    vec2 wuv = fragTexCoord / CELL_SIZE;
    vec2 wuvc = vec2(scale, scale) * vec2(0.5, 0.5);
    float len = length(wuv - wuvc);

    float d = r - len;

    float x = d;
    float res = (sin(x * PI) / x) * 0.25;
    if (abs(res) < 0.01)
        res = 0.0;

    vec2 uv = (wuv + normalize(wuv - wuvc) * res) * CELL_SIZE;

    vec4 color = texture(textureArrays[textureArrayIndex], vec3(uv, texturePageIndex));
    float alphaIn = clamp((radius - len) / 10.0, 0.0, 1.0);
    float alphaOut = 1.0 - clamp(abs(min(len - radiusHiding, 0.0)) / 5.0, 0.0, 1.0);

    float alpha = color.a * min(alphaIn, alphaOut);

    fragColor = vec4(color.rgb, alpha);
}