#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in flat float time;
in flat vec2 scale;
in flat float offset;

layout (location = 0) uniform vec4 color;

out vec4 fragColor;
const float PI = 3.1415926535897932384626433832795;



float quad(vec2 position, vec2 size)
{
    return length(max(abs(position) - size, 0.0));
}

float roundedQuad(vec2 position, vec2 size, float radius)
{
    return length(max(abs(position) - (size * (1.0 - radius)), 0.0)) - radius;
}

float circle(vec2 p, float r)
{
    return length(p) - r;
}

float contour(in float dist, in float w) 
{
    return smoothstep(0.999-w, 0.999+w, dist);
}

float samp(in float dist, in float w) 
{
    return contour(dist, w);
}

float createInnerEdge(vec2 position, vec2 size, float radius)
{
    float dist = 1.0 - roundedQuad(position, size, radius);
    float width = fwidth(dist);
    return clamp(contour(dist, width), 0.0, 1.0);
}

float createOuterEdge(vec2 position, vec2 size)
{
    float dist = 1.0 - quad(position, size);
    float width = fwidth(dist);
    return clamp(contour(dist, width), 0.0, 1.0);
}

float sinedRange(float x, float min, float max)
{ 
    return (sin(x * PI) * 0.5 + 0.5) * (max - min) + min;
}

void main()
{
    vec2 uv = fragTexCoord * scale - (scale * 0.5);
    vec2 q = uv;
    float radius = 0.002;
    vec2 size = (scale * 0.5)-(radius * 1.5);
    float a = createInnerEdge(q, size, radius);
    float b = createInnerEdge(q, size-(radius * 0.5), radius * 0.5);
    float d = a - b;
    
    fragColor = vec4(color.rgb, (d - 0.2) + b * 0.3);//(1.0 - sinedRange(time * 0.5, 0.6, 0.7)));

    //if (uv.y < 0.0)
        //fragColor = vec4(1.0);


    //float outerEdge = createOuterEdge(q, size - 0.0005);
    //float innerEdge = createInnerEdge(q, size-0.005, 0.0005);
    //vec3 lightPos = vec3(7.0, 4.0, 5.0);
    //float light = max(dot(fragNormal, normalize(lightPos)), 0.5);
    //vec3 baseColor = vec3(0.1, 0.5, 0.9);

    //fragColor = vec4(baseColor + 0.2, outerEdge * (1.0 - innerEdge + sinedRange(time, 0.1, 0.2)));

}
