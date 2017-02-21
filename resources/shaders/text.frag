#version 450
//#extension GL_ARB_bindless_texture: enable

in vec3 fragPosition;
in vec2 fragTexCoord;

flat in int unit;
flat in float page;
flat in float shift;
flat in vec4 color;
flat in vec2 texelSize;

layout (location = 0) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

vec4 fetch(vec2 uv)
{
    return texture(textureArrays[0], vec3(uv, page));
}

void main(void)
{
    float gamma = 2.25;
    vec4 current = fetch(fragTexCoord);
    vec4 previous= fetch(fragTexCoord + vec2(-1.0, 0.0) * texelSize.xy);

    current = pow(current, vec4(1.0 / gamma));
    previous= pow(previous, vec4(1.0 / gamma));

    float r = current.r;
    float g = current.g;
    float b = current.b;

    if(shift <= 0.333)
    {
        float z = shift / 0.333;
        r = mix(current.r, previous.b, z);
        g = mix(current.g, current.r,  z);
        b = mix(current.b, current.g,  z);
    }
    else if(shift <= 0.666)
    {
        float z = (shift - 0.33) / 0.333;
        r = mix(previous.b, previous.g, z);
        g = mix(current.r,  previous.b, z);
        b = mix(current.g,  current.r,  z);
    }
    else if(shift < 1.0)
    {
        float z = (shift - 0.66) / 0.334;
        r = mix(previous.g, previous.r, z);
        g = mix(previous.b, previous.g, z);
        b = mix(current.r,  previous.b, z);
    }

   float t = max(max(r,g),b);
   vec4 gcolor = vec4(color.rgb, (r + g +b) / 3.0);
   gcolor = t * gcolor + (1.0 - t) * vec4(r, g, b, min(min(r, g), b));

   fragColor = vec4(gcolor.rgb, color.a * gcolor.a);
   //fragColor = fetch(fragTexCoord);
   //fragColor = vec4(0.0);
   //fragColor = vec4((1.0 + float(unit))/4.0);
   //fragColor = color;
}