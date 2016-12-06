#version 450
#extension GL_ARB_bindless_texture: enable

layout (std140, binding = 0) uniform FrameUniformsDataBuffer
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
    vec2 resolution;
    float near;
    float far;
    float halfFovTangent;
    float time; 
    float pad0;
    float pad1;
} frameUniforms;

layout (std140, binding = 1) uniform gBufferRTAddresses
{
    int rt0Unit;
    int rt1Unit;
    int rt2Unit;
    int depthUnit;
    int pickingUnit;
    int pad0;
    int pad1;
    int pad2;
    float rt0Page;
    float rt1Page;
    float rt2Page;
    float depthPage;
    float pickingPage;
    float pad3;
    float pad4;
    float pad5;
} rtAddresses;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;

layout (location = 0) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

vec4 fetchRt0(vec2 uv)
{
    return texture(textureArrays[rtAddresses.rt0Unit], vec3(uv, rtAddresses.rt0Page));
}

vec4 fetchRt1(vec2 uv)
{
    return texture(textureArrays[rtAddresses.rt1Unit], vec3(uv, rtAddresses.rt1Page));
}

vec4 fetchRt2(vec2 uv)
{
    return texture(textureArrays[rtAddresses.rt2Unit], vec3(uv, rtAddresses.rt2Page));
}

float fetchDepth(vec2 uv)
{
    return texture(textureArrays[rtAddresses.depthUnit], vec3(uv, rtAddresses.depthPage)).r;
}

int fetchPicking(vec2 uv)
{
    float alpha = texture(textureArrays[rtAddresses.pickingUnit], vec3(uv, rtAddresses.pickingPage)).a;
    alpha *= 255.0;
    return int(alpha);
}

vec3 decodeNormal (vec2 enc)
{
    float scale = 1.7777;
    vec3 nn = vec3(enc.xy, 0.0) * vec3(2*scale, 2*scale,0) + vec3(-scale, -scale, 1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

float grayScale(in vec3 color){
    return dot(vec3(0.2126, 0.7152, 0.0722), color);
}

void main()
{
    //normal map
    vec3 n = normalize(fragNormal);
    vec3 t = normalize(fragTangent); 
    vec3 b = cross(n, t);

    mat3 tbn = mat3(t, b, n);
    vec4 normalTexel = vec4(0.5f, 0.5f, 1.0f, 1.0f);
    vec3 normal = normalize(tbn * (normalTexel * 2.0 - 1.0).xyz);

    //vec3 lightDir = mat3(frameUniforms.v) * normalize(-vec3(0.0, 0.0, 1.0));
    vec3 lightDir = -vec3(0.0, 0.0, 1.0);
    vec2 uv = gl_FragCoord.xy/frameUniforms.resolution;
    
    vec3 bufferColor = fetchRt0(uv).xyz;
    float bufferDepth = fetchDepth(uv);
    float myDepth = gl_FragCoord.z;

    float f = myDepth - bufferDepth;

    int picking = fetchPicking(uv);
    bool isTranslating = (picking & 2) == 2;

    vec3 color = vec3(1.0);
    float a = 0.8;

    if (!(myDepth < bufferDepth || isTranslating))
        a = 0.4 * max(dot(normal, -lightDir), 0.4);

    fragColor = vec4(color, a);
}