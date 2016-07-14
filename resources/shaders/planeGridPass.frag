#version 450

uniform float radiusFadeIn;
uniform float radiusFadeOut;
uniform float radiusWave;
uniform vec2 expansionPosition;

uniform int textureArrayIndex;
uniform float texturePageIndex;
uniform sampler2DArray textureArrays[32];
uniform vec4 color;

in vec2 fragTexCoord;
out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
const float CELL_SIZE = 1.0;
const float FADE_IN_DISTANCE = 5.0;
const float FADE_OUT_DISTANCE = 10.0;

void main()
{
    vec2 diff = fragTexCoord - expansionPosition;
    float dist = length(diff);

    float x = radiusWave - dist; // How close to the wave radius the current point is from the expansion position.

    float res = (sin(x * PI) / x) * 0.25;
    if (abs(res) < 0.01)
        res = 0.0;

    vec2 uv = fragTexCoord * (1.0 / CELL_SIZE) + normalize(diff) * res + vec2(0.5);

    vec4 texColor = texture(textureArrays[textureArrayIndex], vec3(uv, texturePageIndex));
    float alphaFadeIn = 1.0 - clamp(abs(min(dist - radiusFadeIn, 0.0)) / FADE_IN_DISTANCE, 0.0, 1.0);
    float alphaFadeOut = clamp((radiusFadeOut - dist) / FADE_OUT_DISTANCE, 0.0, 1.0);

    float alpha = texColor.a * min(alphaFadeIn, alphaFadeOut);
    alpha += alpha * res * 0.3;

    fragColor = vec4(texColor.rgb, alpha) * color;
}