#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2DArray textureArrays[32];
uniform int unit;
uniform float page;
uniform vec2 resolution; 
 
float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

out vec4 fragColor;

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
        color += fetch(uv + vec2(0.0, offset[i] / resolution.x)) * weight[i];
        color += fetch(uv - vec2(0.0, offset[i] / resolution.y)) * weight[i];
    }
    
    fragColor = vec4(color, 1.0);
}